#include <SDL3/SDL.h>
#include <Tactics/Core/Camera.hpp>
#include <Tactics/Core/Engine.hpp>
#include <Tactics/Core/Grid.hpp>
#include <Tactics/Core/InputManager.hpp>
#include <Tactics/Core/Logger.hpp>
#include <Tactics/Core/Tile.hpp>
#include <cstdlib>

namespace
{
    constexpr float CAMERA_SPEED = 1000.0F; // pixels per second
    constexpr float TILE_SIZE = 32.0F;
} // namespace

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
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

    // Load grid from JSON file
    Tactics::Grid grid;
    if (!grid.load_from_json("map.json"))
    {
        Tactics::log_error("Failed to load grid from map.json");
        engine.shutdown();
        return EXIT_FAILURE;
    }

    const int GRID_WIDTH = grid.get_width();
    const int GRID_HEIGHT = grid.get_height();

    // Create camera centered on the grid
    constexpr float VIEWPORT_WIDTH = 1280.0F;
    constexpr float VIEWPORT_HEIGHT = 720.0F;
    const float grid_center_x = (static_cast<float>(GRID_WIDTH) * TILE_SIZE) * 0.5F;
    const float grid_center_y = (static_cast<float>(GRID_HEIGHT) * TILE_SIZE) * 0.5F;

    Tactics::CameraSettings camera_settings;
    camera_settings.position = {grid_center_x, grid_center_y};
    camera_settings.zoom = 1.0F;
    camera_settings.viewport_width = VIEWPORT_WIDTH;
    camera_settings.viewport_height = VIEWPORT_HEIGHT;
    Tactics::Camera camera(camera_settings);

    // Create input manager
    Tactics::InputManager input;

    Tactics::log_info("Grid created: " + std::to_string(GRID_WIDTH) + "x" +
                      std::to_string(GRID_HEIGHT));
    Tactics::log_info("Use WASD or Arrow Keys to move the camera");
    Tactics::log_info("Press Q to zoom out, E to zoom in");
    Tactics::log_info("Press ESC to quit");

    // Main game loop
    bool running = true;
    SDL_Renderer *renderer = engine.get_renderer();
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

        // Handle input for camera movement
        Tactics::Vector2f camera_movement(0.0F, 0.0F);

        if (input.is_key_pressed(SDL_SCANCODE_W) || input.is_key_pressed(SDL_SCANCODE_UP))
        {
            camera_movement.y -= CAMERA_SPEED * delta_time;
        }
        if (input.is_key_pressed(SDL_SCANCODE_S) || input.is_key_pressed(SDL_SCANCODE_DOWN))
        {
            camera_movement.y += CAMERA_SPEED * delta_time;
        }
        if (input.is_key_pressed(SDL_SCANCODE_A) || input.is_key_pressed(SDL_SCANCODE_LEFT))
        {
            camera_movement.x -= CAMERA_SPEED * delta_time;
        }
        if (input.is_key_pressed(SDL_SCANCODE_D) || input.is_key_pressed(SDL_SCANCODE_RIGHT))
        {
            camera_movement.x += CAMERA_SPEED * delta_time;
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

        // Update camera position
        if (camera_movement.x != 0.0F || camera_movement.y != 0.0F)
        {
            camera.translate(camera_movement);
        }

        // Render
        SDL_SetRenderDrawColor(renderer, 0x2E, 0x2E, 0x2E, 0xFF);
        SDL_RenderClear(renderer);

        // Render grid
        grid.render(renderer, camera);

        SDL_RenderPresent(renderer);
    }

    engine.shutdown();

    Tactics::log_info("=== Tactics Engine Shutting Down ===");
    return EXIT_SUCCESS;
}
