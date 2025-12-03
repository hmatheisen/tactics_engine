#include "Tactics/Core/CursorController.hpp"
#include "Tactics/Core/Cursor.hpp"
#include "Tactics/Core/CursorEvents.hpp"
#include "Tactics/Core/InputManager.hpp"

namespace Tactics
{
    namespace
    {
        constexpr float DEFAULT_KEY_REPEAT_INITIAL_DELAY = 0.2F; // Seconds before first repeat
        constexpr float DEFAULT_KEY_REPEAT_RATE = 0.04F;         // Seconds between repeats

        void update_key_state(KeyState &key_state)
        {
            auto &input = InputManager::instance();

            key_state.just_pressed = input.is_key_just_pressed(key_state.scancode);
            key_state.pressed = input.is_key_pressed(key_state.scancode);
            key_state.just_released = input.is_key_just_released(key_state.scancode);
        }
    } // namespace

    CursorController::CursorController()
        : m_key_repeat_initial_delay(DEFAULT_KEY_REPEAT_INITIAL_DELAY),
          m_key_repeat_rate(DEFAULT_KEY_REPEAT_RATE)
    {}

    void CursorController::update(Cursor &cursor, const Vector2i &grid_size, float delta_time)
    {
        update_key_state(m_key_up);
        update_key_state(m_key_down);
        update_key_state(m_key_left);
        update_key_state(m_key_right);

        bool cursor_moved = false;

        const bool any_key_just_pressed = m_key_up.just_pressed || m_key_down.just_pressed ||
                                          m_key_left.just_pressed || m_key_right.just_pressed;
        // Handle first press of a key
        if (any_key_just_pressed)
        {
            m_movement_repeat_timer = m_key_repeat_initial_delay;

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
    }
} // namespace Tactics
