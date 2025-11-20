#include <Tactics/Core/CursorController.hpp>

namespace Tactics
{
    namespace
    {
        constexpr float DEFAULT_KEY_REPEAT_INITIAL_DELAY = 0.3F; // Seconds before first repeat
        constexpr float DEFAULT_KEY_REPEAT_RATE = 0.05F;         // Seconds between repeats

        void update_key_state(KeyState &key_state, const InputManager &input)
        {
            key_state.just_pressed = input.is_key_just_pressed(key_state.scancode);
            key_state.pressed = input.is_key_pressed(key_state.scancode);
            key_state.just_released = input.is_key_just_released(key_state.scancode);
        }
    } // namespace

    CursorController::CursorController()
        : m_key_repeat_initial_delay(DEFAULT_KEY_REPEAT_INITIAL_DELAY),
          m_key_repeat_rate(DEFAULT_KEY_REPEAT_RATE)
    {}

    void CursorController::update(const InputManager &input, Cursor &cursor,
                                  const Vector2i &grid_size, float delta_time)
    {
        update_key_state(m_key_up, input);
        update_key_state(m_key_down, input);
        update_key_state(m_key_left, input);
        update_key_state(m_key_right, input);

        const bool any_key_just_pressed = m_key_up.just_pressed || m_key_down.just_pressed ||
                                          m_key_left.just_pressed || m_key_right.just_pressed;
        // Handle first press of a key
        if (any_key_just_pressed)
        {
            m_movement_repeat_timer = m_key_repeat_initial_delay;

            if (m_key_up.just_pressed)
            {
                cursor.move_up();
            }

            if (m_key_down.just_pressed)
            {
                cursor.move_down();
            }

            if (m_key_left.just_pressed)
            {
                cursor.move_left();
            }

            if (m_key_right.just_pressed)
            {
                cursor.move_right();
            }

            cursor.clamp_to_grid(grid_size);
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
            }

            if (m_key_down.pressed)
            {
                cursor.move_down();
            }

            if (m_key_left.pressed)
            {
                cursor.move_left();
            }

            if (m_key_right.pressed)
            {
                cursor.move_right();
            }

            cursor.clamp_to_grid(grid_size);
        }
    }
} // namespace Tactics
