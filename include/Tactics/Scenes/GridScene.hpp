#pragma once

#include "Tactics/Core/Camera.hpp"
#include "Tactics/Core/CameraController.hpp"
#include "Tactics/Core/Cursor.hpp"
#include "Tactics/Core/CursorController.hpp"
#include "Tactics/Core/EventBus.hpp"
#include "Tactics/Core/Grid.hpp"
#include "Tactics/Core/IGridRepository.hpp"
#include "Tactics/Core/Scene.hpp"

#include <SDL3/SDL.h>
#include <string>

namespace Tactics
{
    class GridScene : public Scene, public Publisher
    {
    public:
        explicit GridScene(IGridRepository *repository, std::string map_name = "default");
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
        Cursor m_cursor;
        CameraController m_camera_controller;
        CursorController m_cursor_controller;

        IGridRepository *m_repository;
        std::string m_map_name;
        bool m_running;

        static constexpr float TILE_SIZE = 32.0F;
        static constexpr float VIEWPORT_WIDTH = 1280.0F;
        static constexpr float VIEWPORT_HEIGHT = 720.0F;
    };
} // namespace Tactics
