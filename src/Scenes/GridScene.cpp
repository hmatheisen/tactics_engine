#include "Tactics/Scenes/GridScene.hpp"
#include "Tactics/Core/CursorEvents.hpp"
#include "Tactics/Core/InputManager.hpp"
#include "Tactics/Core/Logger.hpp"

namespace Tactics
{
    GridScene::GridScene(IGridRepository *repository, std::string map_name)
        : m_repository(repository), m_map_name(std::move(map_name)), m_running(true)
    {}

    auto GridScene::on_enter() -> bool
    {
        log_info("Entering GridScene");

        if (m_repository == nullptr)
        {
            log_error("Repository is null");
            return false;
        }

        // Load grid from repository
        auto grid_opt = m_repository->load_map(m_map_name);
        if (!grid_opt.has_value())
        {
            log_error("Failed to load grid from repository: " + m_map_name);
            return false;
        }
        m_grid = std::move(grid_opt.value());

        const int grid_width = m_grid.get_width();
        const int grid_height = m_grid.get_height();

        // Create cursor at center of grid
        m_cursor = Cursor({grid_width / 2, grid_height / 2});

        // Publish initial cursor position so camera has correct state before updates
        publish(CursorEvents::Moved{m_cursor.get_position()});

        // Create camera at center of grid (independent of cursor position)
        const float grid_center_x = (static_cast<float>(grid_width) * TILE_SIZE) * 0.5F;
        const float grid_center_y = (static_cast<float>(grid_height) * TILE_SIZE) * 0.5F;
        m_camera = Camera({.position = {grid_center_x, grid_center_y},
                           .zoom = 1.0F,
                           .viewport_width = VIEWPORT_WIDTH,
                           .viewport_height = VIEWPORT_HEIGHT});

        log_info("Grid created: " + std::to_string(grid_width) + "x" + std::to_string(grid_height));
        log_info("Use WASD or Arrow Keys to move the cursor");
        log_info("Press Q to zoom out, E to zoom in");
        log_info("Press ESC to quit");

        return true;
    }

    void GridScene::on_exit()
    {
        log_info("Exiting GridScene");
    }

    namespace
    {
        constexpr float ZOOM_OUT_FACTOR = 0.9F;
        constexpr float ZOOM_IN_FACTOR = 1.1F;
    } // namespace
    void GridScene::update(float delta_time)
    {
        auto &input = InputManager::instance();

        // Update cursor controller
        const Vector2i grid_size(m_grid.get_width(), m_grid.get_height());
        m_cursor_controller.update(m_cursor, grid_size, delta_time);

        // Handle zoom
        if (input.is_key_just_pressed(SDL_SCANCODE_Q))
        {
            m_camera.set_zoom(m_camera.get_zoom() * ZOOM_OUT_FACTOR); // Zoom out
        }
        if (input.is_key_just_pressed(SDL_SCANCODE_E))
        {
            m_camera.set_zoom(m_camera.get_zoom() * ZOOM_IN_FACTOR); // Zoom in
        }

        // Handle escape to quit
        if (input.is_key_just_pressed(SDL_SCANCODE_ESCAPE))
        {
            m_running = false;
        }

        // Update camera controller (edge scrolling)
        m_camera_controller.update(m_camera, TILE_SIZE);
    }

    namespace
    {
        constexpr uint8_t BACKGROUND_COLOR_R = 0x2E;
        constexpr uint8_t BACKGROUND_COLOR_G = 0x2E;
        constexpr uint8_t BACKGROUND_COLOR_B = 0x2E;
        constexpr uint8_t BACKGROUND_COLOR_A = 0xFF;
    } // namespace
    void GridScene::render(SDL_Renderer *renderer)
    {
        if (renderer == nullptr)
        {
            return;
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B,
                               BACKGROUND_COLOR_A);
        SDL_RenderClear(renderer);

        // Render grid
        m_grid.render(renderer, m_camera);

        // Render cursor
        m_cursor.render(renderer, m_camera, TILE_SIZE);
    }

    auto GridScene::should_exit() const -> bool
    {
        return !m_running;
    }
} // namespace Tactics
