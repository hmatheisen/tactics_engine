#pragma once

#include "Tactics/Core/Camera.hpp"
#include "Tactics/Core/Cursor.hpp"
#include "Tactics/Core/EventBus.hpp"
#include "Tactics/Core/Vector2.hpp"
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

        [[nodiscard]] auto update(Camera &camera, Cursor &cursor, float delta_time,
                                  const Vector2i &grid_size, float tile_size) -> bool;

    private:
        static constexpr float PAN_SPEED_TILES_PER_SECOND = 10.0F;

        bool m_was_panning = false;

        static void apply_camera_movement(Camera &camera, float movement_distance, bool move_up,
                                          bool move_down, bool move_left, bool move_right);
        static void ensure_cursor_visible(Cursor &cursor, const Camera &camera,
                                          const Vector2i &grid_size, float tile_size);
        static void center_cursor_in_view(Cursor &cursor, const Camera &camera,
                                          const Vector2i &grid_size, float tile_size);
    };
} // namespace Tactics
