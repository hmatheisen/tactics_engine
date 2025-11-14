#include <SDL3/SDL.h>
#include <Tactics/Core/Camera.hpp>
#include <Tactics/Core/Cursor.hpp>
#include <Tactics/Core/Engine.hpp>
#include <Tactics/Core/Grid.hpp>
#include <Tactics/Core/InputManager.hpp>
#include <Tactics/Core/Logger.hpp>
#include <Tactics/Core/Tile.hpp>
#include <cstdlib>

namespace
{
    constexpr float TILE_SIZE = 32.0F;
    constexpr float KEY_REPEAT_INITIAL_DELAY = 0.3F; // Seconds before first repeat
    constexpr float KEY_REPEAT_RATE = 0.05F;         // Seconds between repeats
    constexpr float EDGE_SCROLL_THRESHOLD = 100.0F;  // Pixels from edge to trigger camera movement
} // namespace

// NOLINTBEGIN(readability-function-cognitive-complexity, cppcoreguidelines-avoid-magic-numbers,
// readability-magic-numbers)
auto main() -> int
{
    // Initialize logger
    Tactics::Logger::instance().set_level(Tactics::LogLevel::Info);
    Tactics::Logger::instance().set_file_logging(true, "tactics.log");

    Tactics::log_info("=== Tactics Engine Starting ===");

    Tactics::Engine engine;

    if (!engine.initialize())
    {
        Tactics::log_error("Failed to initialize engine");
        return EXIT_FAILURE;
    }

    // Load grid from Boost serialization file
    Tactics::Grid grid;
    if (!grid.load_from_file("map.bin"))
    {
        Tactics::log_info("map.bin not found, creating new grid from map.json");
        // Fallback to JSON if binary file doesn't exist
        if (!grid.load_from_json("map.json"))
        {
            Tactics::log_error("Failed to load grid from map.json");
            engine.shutdown();
            return EXIT_FAILURE;
        }
        // Save as binary for next time
        if (!grid.save_to_file("map.bin"))
        {
            Tactics::log_error("Failed to save grid to map.bin");
            engine.shutdown();
            return EXIT_FAILURE;
        }

        Tactics::log_info("Grid saved to map.bin");
    }

    const int GRID_WIDTH = grid.get_width();
    const int GRID_HEIGHT = grid.get_height();

    // Create cursor at center of grid
    Tactics::Cursor cursor({GRID_WIDTH / 2, GRID_HEIGHT / 2});

    // Create camera at center of grid (independent of cursor position)
    constexpr float VIEWPORT_WIDTH = 1280.0F;
    constexpr float VIEWPORT_HEIGHT = 720.0F;
    const float grid_center_x = (static_cast<float>(GRID_WIDTH) * TILE_SIZE) * 0.5F;
    const float grid_center_y = (static_cast<float>(GRID_HEIGHT) * TILE_SIZE) * 0.5F;
    Tactics::Camera camera({.position = {grid_center_x, grid_center_y},
                            .zoom = 1.0F,
                            .viewport_width = VIEWPORT_WIDTH,
                            .viewport_height = VIEWPORT_HEIGHT});

    // Create input manager
    Tactics::InputManager input;

    Tactics::log_info("Grid created: " + std::to_string(GRID_WIDTH) + "x" +
                      std::to_string(GRID_HEIGHT));
    Tactics::log_info("Use WASD or Arrow Keys to move the cursor");
    Tactics::log_info("Press Q to zoom out, E to zoom in");
    Tactics::log_info("Press ESC to quit");

    // Main game loop
    bool running = true;
    SDL_Renderer *renderer = engine.get_renderer();

    // Key repeat tracking (single timer for all movement keys)
    float key_repeat_timer = 0.0F;
    bool any_movement_key_was_pressed = false;

    Uint64 last_frame_time = SDL_GetPerformanceCounter();
    const Uint64 performance_frequency = SDL_GetPerformanceFrequency();

    while (running)
    {
        // Calculate delta time
        Uint64 current_time = SDL_GetPerformanceCounter();
        float delta_time = static_cast<float>(current_time - last_frame_time) /
                           static_cast<float>(performance_frequency);
        last_frame_time = current_time;

        // Update input
        input.update();

        // Process events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            input.process_event(event);

            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
        }

        // Handle input for cursor movement with key repeat
        const bool key_up =
            input.is_key_pressed(SDL_SCANCODE_W) || input.is_key_pressed(SDL_SCANCODE_UP);
        const bool key_down =
            input.is_key_pressed(SDL_SCANCODE_S) || input.is_key_pressed(SDL_SCANCODE_DOWN);
        const bool key_left =
            input.is_key_pressed(SDL_SCANCODE_A) || input.is_key_pressed(SDL_SCANCODE_LEFT);
        const bool key_right =
            input.is_key_pressed(SDL_SCANCODE_D) || input.is_key_pressed(SDL_SCANCODE_RIGHT);

        const bool any_movement_key_pressed = key_up || key_down || key_left || key_right;

        // Update key repeat timer
        if (any_movement_key_pressed)
        {
            if (!any_movement_key_was_pressed || key_repeat_timer <= 0.0F)
            {
                // Process movement based on which keys are pressed
                if (key_up)
                {
                    cursor.move_up();
                }
                if (key_down)
                {
                    cursor.move_down();
                }
                if (key_left)
                {
                    cursor.move_left();
                }
                if (key_right)
                {
                    cursor.move_right();
                }

                cursor.clamp_to_grid({GRID_WIDTH, GRID_HEIGHT});

                // Reset timer
                key_repeat_timer =
                    any_movement_key_was_pressed ? KEY_REPEAT_RATE : KEY_REPEAT_INITIAL_DELAY;
                any_movement_key_was_pressed = true;
            }
            else
            {
                key_repeat_timer -= delta_time;
            }
        }
        else
        {
            any_movement_key_was_pressed = false;
            key_repeat_timer = 0.0F;
        }

        // Handle zoom
        if (input.is_key_just_pressed(SDL_SCANCODE_Q))
        {
            camera.set_zoom(camera.get_zoom() * 0.9F); // Zoom out
        }
        if (input.is_key_just_pressed(SDL_SCANCODE_E))
        {
            camera.set_zoom(camera.get_zoom() * 1.1F); // Zoom in
        }

        // Handle escape to quit
        if (input.is_key_just_pressed(SDL_SCANCODE_ESCAPE))
        {
            running = false;
        }

        // Update camera to scroll when cursor is near viewport edges (minimal movement to keep
        // cursor visible)
        const Tactics::Vector2i current_cursor_pos = cursor.get_position();
        const float cursor_world_x = static_cast<float>(current_cursor_pos.x) * TILE_SIZE;
        const float cursor_world_y = static_cast<float>(current_cursor_pos.y) * TILE_SIZE;
        const Tactics::Vector2f cursor_screen_pos =
            camera.world_to_screen({cursor_world_x, cursor_world_y});

        // Calculate camera movement based on cursor proximity to edges
        Tactics::Vector2f camera_movement(0.0F, 0.0F);
        const float viewport_width = camera.get_viewport_width();
        const float viewport_height = camera.get_viewport_height();

        // Check left edge - only scroll if cursor is too close to edge
        if (cursor_screen_pos.x < EDGE_SCROLL_THRESHOLD)
        {
            // Calculate how much we need to move to keep cursor at threshold distance
            const float distance_to_move = EDGE_SCROLL_THRESHOLD - cursor_screen_pos.x;
            // Convert screen distance to world distance
            const float world_distance = distance_to_move / camera.get_zoom();
            camera_movement.x -= world_distance;
        }

        // Check right edge
        if (cursor_screen_pos.x > viewport_width - EDGE_SCROLL_THRESHOLD)
        {
            // Calculate how much we need to move to keep cursor at threshold distance
            const float distance_to_move =
                cursor_screen_pos.x - (viewport_width - EDGE_SCROLL_THRESHOLD);
            // Convert screen distance to world distance
            const float world_distance = distance_to_move / camera.get_zoom();
            camera_movement.x += world_distance;
        }

        // Check top edge
        if (cursor_screen_pos.y < EDGE_SCROLL_THRESHOLD)
        {
            // Calculate how much we need to move to keep cursor at threshold distance
            const float distance_to_move = EDGE_SCROLL_THRESHOLD - cursor_screen_pos.y;
            // Convert screen distance to world distance
            const float world_distance = distance_to_move / camera.get_zoom();
            camera_movement.y -= world_distance;
        }

        // Check bottom edge
        if (cursor_screen_pos.y > viewport_height - EDGE_SCROLL_THRESHOLD)
        {
            // Calculate how much we need to move to keep cursor at threshold distance
            const float distance_to_move =
                cursor_screen_pos.y - (viewport_height - EDGE_SCROLL_THRESHOLD);
            // Convert screen distance to world distance
            const float world_distance = distance_to_move / camera.get_zoom();
            camera_movement.y += world_distance;
        }

        // Apply camera movement (moves just enough to keep cursor within threshold)
        if (camera_movement.x != 0.0F || camera_movement.y != 0.0F)
        {
            camera.translate(camera_movement);
        }

        // Render
        SDL_SetRenderDrawColor(renderer, 0x2E, 0x2E, 0x2E, 0xFF);
        SDL_RenderClear(renderer);

        // Render grid
        grid.render(renderer, camera);

        // Render cursor
        cursor.render(renderer, camera, TILE_SIZE);

        SDL_RenderPresent(renderer);
    }

    engine.shutdown();

    Tactics::log_info("=== Tactics Engine Shutting Down ===");
    return EXIT_SUCCESS;
}
// NOLINTEND(readability-function-cognitive-complexity, cppcoreguidelines-avoid-magic-numbers,
// readability-magic-numbers)
