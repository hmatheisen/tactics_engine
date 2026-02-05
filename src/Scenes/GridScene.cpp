#include "Tactics/Scenes/GridScene.hpp"
#include "Tactics/Core/CursorEvents.hpp"
#include "Tactics/Core/InputManager.hpp"
#include "Tactics/Core/Logger.hpp"
#include "Tactics/Core/MapGenerator.hpp"

namespace Tactics
{
    GridScene::GridScene(IGridRepository *repository, IUnitRepository *unit_repository,
                         std::string map_name)
        : m_repository(repository), m_unit_repository(unit_repository),
          m_map_name(std::move(map_name)), m_running(true)
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

        int grid_width = 0;
        int grid_height = 0;

        grid_width = m_grid.get_width();
        grid_height = m_grid.get_height();

        // Create cursor at center of grid
        m_cursor = Cursor({grid_width / 2, grid_height / 2});

        if (m_unit_repository != nullptr)
        {
            std::vector<Unit> units = m_unit_repository->load_units(m_map_name);
            if (units.empty())
            {
                m_unit_controller.reset_for_grid(m_grid, Vector2i{grid_width / 2, grid_height / 2});
            }
            else
            {
                m_unit_controller.set_units(m_grid, std::move(units));
            }
        }
        else
        {
            m_unit_controller.reset_for_grid(m_grid, Vector2i{grid_width / 2, grid_height / 2});
        }

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
        log_info("Hold Enter and use WASD to move the camera");
        log_info("Press Q to zoom out, E to zoom in");
        log_info("Press ESC to quit");

        return true;
    }

    void GridScene::on_exit()
    {
        if (!m_repository->save_map(m_map_name, m_grid))
        {
            log_error("Failed to save map");
        }

        if (m_unit_repository != nullptr)
        {
            if (!m_unit_repository->save_units(m_map_name, m_unit_controller.get_units()))
            {
                log_error("Failed to save units");
            }
        }

        log_info("Exiting GridScene");
    }

    void GridScene::update(float delta_time)
    {
        auto &input = InputManager::instance();

        const Vector2i grid_size(m_grid.get_width(), m_grid.get_height());
        const bool camera_pan_active =
            m_camera_pan_controller.update(m_camera, m_cursor, delta_time, grid_size, TILE_SIZE);
        if (!camera_pan_active)
        {
            // Update cursor controller
            m_cursor_controller.update(m_cursor, grid_size, delta_time);
        }

        // Handle Zoom
        m_zoom_controller.update(m_camera, delta_time);

        // Handle map regeneration
        // NOTE: temporary to test map generation
        if (input.is_key_just_pressed(SDL_SCANCODE_G))
        {
            log_info("Regenerating map with new seed");

            auto config_opt = m_repository->load_generator_config(m_map_name);
            GeneratorConfig config = config_opt.value_or(GeneratorConfig::default_config());
            config.width = m_grid.get_width();
            config.height = m_grid.get_height();
            config.seed += 1;

            MapGenerator generator(config);
            m_grid = generator.generate();

            if (!m_repository->save_map(m_map_name, m_grid))
            {
                log_error("Failed to save regenerated map");
            }
            if (!m_repository->save_generator_config(m_map_name, config))
            {
                log_error("Failed to save generator config");
            }

            int grid_width = 0;
            int grid_height = 0;

            grid_width = m_grid.get_width();
            grid_height = m_grid.get_height();
            m_cursor.set_position({grid_width / 2, grid_height / 2});
            publish(CursorEvents::Moved{m_cursor.get_position()});

            m_unit_controller.on_grid_changed(m_grid);
        }

        m_unit_controller.update(m_grid, m_cursor);

        // Handle escape to quit
        if (input.is_key_just_pressed(SDL_SCANCODE_ESCAPE))
        {
            m_running = false;
        }

        // Update camera controller (edge scrolling)
        if (!camera_pan_active)
        {
            m_camera_controller.update(m_camera, TILE_SIZE);
        }
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

        m_unit_controller.render(renderer, m_camera, TILE_SIZE, m_grid);

        // Render cursor
        m_cursor.render(renderer, m_camera, TILE_SIZE);
    }

    auto GridScene::should_exit() const -> bool
    {
        return !m_running;
    }
} // namespace Tactics
