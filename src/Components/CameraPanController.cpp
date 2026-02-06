#include "Tactics/Components/CameraPanController.hpp"
#include "Tactics/Components/CursorEvents.hpp"
#include "Tactics/Core/InputManager.hpp"
#include <algorithm>
#include <cmath>

namespace Tactics
{
    auto CameraPanController::update(Camera &camera, Cursor &cursor, float delta_time,
                                     const Vector2i &grid_size, float tile_size) -> bool
    {
        auto &input = InputManager::instance();
        const bool pan_modifier = input.is_key_pressed(SDL_SCANCODE_RETURN) ||
                                  input.is_key_pressed(SDL_SCANCODE_KP_ENTER);

        if (!pan_modifier)
        {
            if (m_was_panning)
            {
                center_cursor_in_view(cursor, camera, grid_size, tile_size);
            }
            m_was_panning = false;
            return false;
        }

        m_was_panning = true;

        const bool up_pressed = input.is_key_pressed(SDL_SCANCODE_W);
        const bool down_pressed = input.is_key_pressed(SDL_SCANCODE_S);
        const bool left_pressed = input.is_key_pressed(SDL_SCANCODE_A);
        const bool right_pressed = input.is_key_pressed(SDL_SCANCODE_D);
        const bool any_pressed = up_pressed || down_pressed || left_pressed || right_pressed;

        if (any_pressed)
        {
            const float zoom = std::max(camera.get_zoom(), 0.01F);
            const float movement_distance =
                (tile_size * PAN_SPEED_TILES_PER_SECOND * delta_time) / zoom;
            apply_camera_movement(camera, movement_distance, up_pressed, down_pressed, left_pressed,
                                  right_pressed);
        }

        return true;
    }

    void CameraPanController::apply_camera_movement(Camera &camera, float movement_distance,
                                                    bool move_up, bool move_down, bool move_left,
                                                    bool move_right)
    {
        Vector2f movement_dir(0.0F, 0.0F);

        if (move_up)
        {
            movement_dir.y -= 1.0F;
        }
        if (move_down)
        {
            movement_dir.y += 1.0F;
        }
        if (move_left)
        {
            movement_dir.x -= 1.0F;
        }
        if (move_right)
        {
            movement_dir.x += 1.0F;
        }

        if (movement_dir.x != 0.0F || movement_dir.y != 0.0F)
        {
            movement_dir = movement_dir.normalized();
            const Vector2f movement = movement_dir * movement_distance;
            camera.translate(movement);
        }
    }

    void CameraPanController::center_cursor_in_view(Cursor &cursor, const Camera &camera,
                                                    const Vector2i &grid_size, float tile_size)
    {
        const Vector2f camera_pos = camera.get_position();
        const int center_x = static_cast<int>(std::round(camera_pos.x / tile_size));
        const int center_y = static_cast<int>(std::round(camera_pos.y / tile_size));
        const Vector2i clamped_pos{std::clamp(center_x, 0, grid_size.x - 1),
                                   std::clamp(center_y, 0, grid_size.y - 1)};

        if (clamped_pos != cursor.get_position())
        {
            cursor.set_position(clamped_pos);
            publish(CursorEvents::Moved{cursor.get_position()});
        }
    }
} // namespace Tactics
