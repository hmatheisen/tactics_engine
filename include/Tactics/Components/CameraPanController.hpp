#pragma once

#include "Tactics/Components/Camera.hpp"
#include "Tactics/Components/Cursor.hpp"
#include "Tactics/Core/EventBus.hpp"
#include <SDL3/SDL.h>

namespace Tactics
{
    class CameraPanController : public Publisher
    {
    public:
        CameraPanController() = default;
        ~CameraPanController() = default;

        CameraPanController(const CameraPanController &) = delete;
        auto operator=(const CameraPanController &) -> CameraPanController & = delete;

        CameraPanController(CameraPanController &&) = delete;
        auto operator=(CameraPanController &&) -> CameraPanController & = delete;

        [[nodiscard]] auto update(Camera &camera, Cursor &cursor, float delta_time) -> bool;

    private:
        static constexpr float PAN_SPEED_WORLD_PER_SECOND = 1000.0F;

        bool m_was_panning = false;

        static void apply_camera_movement(Camera &camera, float movement_distance, bool move_up,
                                          bool move_down, bool move_left, bool move_right);
        static auto is_cursor_in_view(const Cursor &cursor, const Camera &camera) -> bool;
        static void center_cursor_in_view(Cursor &cursor, const Camera &camera);
    };
} // namespace Tactics
