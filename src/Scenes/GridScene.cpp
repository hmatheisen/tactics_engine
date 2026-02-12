#include "Tactics/Scenes/GridScene.hpp"

#include "Tactics/Core/Events.hpp"
#include "Tactics/Core/InputManager.hpp"
#include "Tactics/Core/Logger.hpp"
#include "Tactics/Core/MapGenerator.hpp"
#include "Tactics/Renderers/CursorRenderer.hpp"
#include "Tactics/Renderers/GridRenderer.hpp"

namespace Tactics
{
    GridScene::GridScene(IGridRepository *repository, IUnitRepository *unit_repository,
                         std::string map_name)
        : m_grid_repository(repository), m_unit_repository(unit_repository),
          m_map_name(std::move(map_name)), m_running(true)
    {}

    namespace
    {
        auto handle_map_regenerated(const Events::MapRegenerated &event) -> void
        {
            log_info("Map regenerated: " + event.map_name);
            log_info("Seed: " + std::to_string(event.seed));
        }
    } // namespace

    auto GridScene::on_enter() -> bool
    {
        log_info("Entering GridScene");

        if (m_grid_repository == nullptr)
        {
            log_error("Grid repository is null");
            return false;
        }

        // Load grid from repository
        auto grid_opt = m_grid_repository->load_map(m_map_name);
        if (!grid_opt.has_value())
        {
            log_error("Failed to load grid from repository: " + m_map_name);
            return false;
        }
        m_grid = std::move(grid_opt.value());

        const int grid_width = m_grid.get_width();
        const int grid_height = m_grid.get_height();

        // Create cursor at center of grid
        m_cursor.set_position(Vector2i{grid_width / 2, grid_height / 2});

        if (m_unit_repository == nullptr)
        {
            log_error("Unit repository is null");
            return false;
        }

        std::vector<Unit> units = m_unit_repository->load_units(m_map_name);
        m_unit_controller.set_units(m_grid, std::move(units));

        // Publish initial cursor position so camera has correct state before updates
        const Vector2i cursor_grid_pos = m_cursor.get_position();
        const Vector2f cursor_world_pos = m_cursor.get_world_position();
        publish(Events::CursorMoved{.grid_position = GridPos{cursor_grid_pos},
                                    .world_position = WorldPos{cursor_world_pos}});
        publish(
            Events::MapLoaded{.map_name = m_map_name, .width = grid_width, .height = grid_height});

        // Create camera at center of grid (independent of cursor position)
        const float grid_center_x = (static_cast<float>(grid_width) * m_config.tile_size) * 0.5F;
        const float grid_center_y = (static_cast<float>(grid_height) * m_config.tile_size) * 0.5F;
        m_camera = Camera({.position = {grid_center_x, grid_center_y},
                           .zoom = 1.0F,
                           .viewport_width = m_config.viewport_width,
                           .viewport_height = m_config.viewport_height});

        m_map_regenerated_subscription_id =
            subscribe<Events::MapRegenerated>(handle_map_regenerated);

        log_info("Grid created: " + std::to_string(grid_width) + "x" + std::to_string(grid_height));
        log_info("Use WASD or Arrow Keys to move the cursor");
        log_info("Hold Enter and use WASD to move the camera");
        log_info("Press Q to zoom out, E to zoom in");
        log_info("Press G to regenerate the map");
        log_info("Press SPACE to select a unit and validate the move");
        log_info("Press ESC to quit");

        return true;
    }

    void GridScene::on_exit()
    {
        if (m_grid_repository != nullptr && !m_grid_repository->save_map(m_map_name, m_grid))
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

        unsubscribe<Events::MapRegenerated>(m_map_regenerated_subscription_id);

        log_info("Exiting GridScene");
    }

    void GridScene::update(float delta_time)
    {
        auto &input = InputManager::instance();

        const Vector2i grid_size(m_grid.get_width(), m_grid.get_height());
        const bool camera_pan_active =
            m_camera_pan_controller.update(m_camera, m_cursor, delta_time);
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

            auto config_opt = m_grid_repository->load_generator_config(m_map_name);
            GeneratorConfig config = config_opt.value_or(GeneratorConfig::default_config());
            config.width = m_grid.get_width();
            config.height = m_grid.get_height();
            config.seed += 1;

            MapGenerator generator(config);
            m_grid = generator.generate();

            if (!m_grid_repository->save_map(m_map_name, m_grid))
            {
                log_error("Failed to save regenerated map");
            }
            if (!m_grid_repository->save_generator_config(m_map_name, config))
            {
                log_error("Failed to save generator config");
            }

            const int new_grid_width = m_grid.get_width();
            const int new_grid_height = m_grid.get_height();
            m_cursor.set_position(Vector2i{new_grid_width / 2, new_grid_height / 2});

            const Vector2i cursor_grid_pos = m_cursor.get_position();
            const Vector2f cursor_world_pos = m_cursor.get_world_position();
            publish(Events::CursorMoved{.grid_position = GridPos{cursor_grid_pos},
                                        .world_position = WorldPos{cursor_world_pos}});
            publish(Events::MapRegenerated{.map_name = m_map_name, .seed = config.seed});

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
            m_camera_controller.update(m_camera);
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
        const bool grid_rendered =
            GridRenderer::render(renderer, m_grid, m_camera, m_config.tile_size);
        (void)grid_rendered;

        m_unit_controller.render(renderer, m_camera, m_config.tile_size, m_grid);

        // Render cursor
        const bool cursor_rendered =
            CursorRenderer::render(renderer, m_cursor, m_camera, m_config.tile_size);
        (void)cursor_rendered;
    }

    auto GridScene::should_exit() const -> bool
    {
        return !m_running;
    }
} // namespace Tactics
