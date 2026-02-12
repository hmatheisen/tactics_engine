#pragma once

#include "Tactics/Components/Camera.hpp"
#include "Tactics/Components/CameraController.hpp"
#include "Tactics/Components/CameraPanController.hpp"
#include "Tactics/Components/Cursor.hpp"
#include "Tactics/Components/CursorController.hpp"
#include "Tactics/Components/Grid.hpp"
#include "Tactics/Components/UnitController.hpp"
#include "Tactics/Components/ZoomController.hpp"
#include "Tactics/Core/EventBus.hpp"
#include "Tactics/Core/IGridRepository.hpp"
#include "Tactics/Core/IUnitRepository.hpp"
#include "Tactics/Core/Scene.hpp"

#include <SDL3/SDL.h>
#include <string>

namespace Tactics
{
    class GridScene : public Scene, public Publisher
    {
    public:
        explicit GridScene(IGridRepository *repository, IUnitRepository *unit_repository,
                           std::string map_name = "default");
        ~GridScene() override = default;

        // Delete copy constructor and assignment operator
        GridScene(const GridScene &) = delete;
        auto operator=(const GridScene &) -> GridScene & = delete;

        // Delete move constructor and assignment operator
        GridScene(GridScene &&) = delete;
        auto operator=(GridScene &&) -> GridScene & = delete;

        // Scene interface
        auto on_enter() -> bool override;
        void on_exit() override;
        void update(float delta_time) override;
        void render(SDL_Renderer *renderer) override;
        [[nodiscard]] auto should_exit() const -> bool override;

    private:
        Grid m_grid;
        Camera m_camera;
        Cursor m_cursor{TILE_SIZE};
        CameraController m_camera_controller;
        CameraPanController m_camera_pan_controller;
        CursorController m_cursor_controller;
        UnitController m_unit_controller;
        ZoomController m_zoom_controller;

        IGridRepository *m_repository = nullptr;
        IUnitRepository *m_unit_repository = nullptr;
        std::string m_map_name;
        bool m_running = false;

        static constexpr float TILE_SIZE = 32.0F;
        static constexpr float VIEWPORT_WIDTH = 1280.0F;
        static constexpr float VIEWPORT_HEIGHT = 720.0F;
    };
} // namespace Tactics
