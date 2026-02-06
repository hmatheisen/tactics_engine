#include "Tactics/Components/CursorController.hpp"
#include "Tactics/Components/Cursor.hpp"
#include "Tactics/Components/CursorEvents.hpp"
#include "Tactics/Core/InputManager.hpp"

namespace Tactics
{
    namespace
    {
        void update_key_state(KeyState &key_state)
        {
            auto &input = InputManager::instance();

            key_state.just_pressed = input.is_key_just_pressed(key_state.scancode);
            key_state.pressed = input.is_key_pressed(key_state.scancode);
            key_state.just_released = input.is_key_just_released(key_state.scancode);
        }
    } // namespace

    void CursorController::update(Cursor &cursor, const Vector2i &grid_size, float delta_time)
    {
        update_key_state(m_key_up);
        update_key_state(m_key_down);
        update_key_state(m_key_left);
        update_key_state(m_key_right);
        update_key_state(m_key_select);

        if (m_key_select.pressed)
        {
            handle_pressed(cursor, grid_size);
            return;
        }

        const bool any_key_just_pressed = m_key_up.just_pressed || m_key_down.just_pressed ||
                                          m_key_left.just_pressed || m_key_right.just_pressed;
        // Handle first press of a key
        if (any_key_just_pressed)
        {
            m_movement_repeat_timer = m_key_repeat_initial_delay;

            handle_just_pressed(cursor, grid_size);
            return;
        }

        const bool any_key_pressed =
            m_key_up.pressed || m_key_down.pressed || m_key_left.pressed || m_key_right.pressed;
        if (!any_key_pressed)
        {
            return;
        }

        m_movement_repeat_timer -= delta_time;
        if (m_movement_repeat_timer <= 0.0F)
        {
            m_movement_repeat_timer = m_key_repeat_rate;

            handle_pressed(cursor, grid_size);
        }
    }

    void CursorController::handle_just_pressed(Cursor &cursor, const Vector2i &grid_size) const
    {
        bool cursor_moved = false;

        if (m_key_up.just_pressed)
        {
            cursor.move_up();
            cursor_moved = true;
        }

        if (m_key_down.just_pressed)
        {
            cursor.move_down();
            cursor_moved = true;
        }

        if (m_key_left.just_pressed)
        {
            cursor.move_left();
            cursor_moved = true;
        }

        if (m_key_right.just_pressed)
        {
            cursor.move_right();
            cursor_moved = true;
        }

        if (cursor_moved)
        {
            cursor.clamp_to_grid(grid_size);
            publish(CursorEvents::Moved{cursor.get_position()});
        }
    }

    void CursorController::handle_pressed(Cursor &cursor, const Vector2i &grid_size) const
    {
        bool cursor_moved = false;

        if (m_key_up.pressed)
        {
            cursor.move_up();
            cursor_moved = true;
        }

        if (m_key_down.pressed)
        {
            cursor.move_down();
            cursor_moved = true;
        }

        if (m_key_left.pressed)
        {
            cursor.move_left();
            cursor_moved = true;
        }

        if (m_key_right.pressed)
        {
            cursor.move_right();
            cursor_moved = true;
        }

        if (cursor_moved)
        {
            cursor.clamp_to_grid(grid_size);
            publish(CursorEvents::Moved{cursor.get_position()});
        }
    }

} // namespace Tactics
