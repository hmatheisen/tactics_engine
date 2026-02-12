#include "Tactics/Components/CameraPanController.hpp"
#include "Tactics/Core/Events.hpp"
#include "Tactics/Core/InputManager.hpp"

#include <algorithm>

namespace Tactics
{
    auto CameraPanController::update(Camera &camera, Cursor &cursor, float delta_time) -> bool
    {
        auto &input = InputManager::instance();
        const bool pan_modifier = input.is_key_pressed(SDL_SCANCODE_RETURN) ||
                                  input.is_key_pressed(SDL_SCANCODE_KP_ENTER);

        if (!pan_modifier)
        {
            if (m_was_panning && !is_cursor_in_view(cursor, camera))
            {
                center_cursor_in_view(cursor, camera);
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
            const float movement_distance = (PAN_SPEED_WORLD_PER_SECOND * delta_time) / zoom;

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

    auto CameraPanController::is_cursor_in_view(const Cursor &cursor, const Camera &camera) -> bool
    {
        const Vector2f cursor_world_position = cursor.get_world_position();
        const Vector2f camera_viewport_size = camera.get_viewport_size();
        const Vector2f cursor_screen_position = camera.world_to_screen(cursor_world_position);

        return cursor_screen_position.x >= 0.0F &&
               cursor_screen_position.x <= camera_viewport_size.x &&
               cursor_screen_position.y >= 0.0F &&
               cursor_screen_position.y <= camera_viewport_size.y;
    }

    void CameraPanController::center_cursor_in_view(Cursor &cursor, const Camera &camera)
    {
        const Rectf view_rect = camera.get_view_rect();
        const Vector2f center_world = view_rect.center();

        cursor.set_world_position(center_world);
        const Vector2i grid_position = cursor.get_position();

        publish(Events::CursorMoved{.grid_position = GridPos{grid_position},
                                    .world_position = WorldPos{center_world}});
    }
} // namespace Tactics
