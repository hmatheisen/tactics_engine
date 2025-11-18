#include "Tactics/Core/Logger.hpp"
#include <Tactics/Core/CursorController.hpp>
#include <string>

namespace Tactics
{
    namespace
    {
        constexpr float DEFAULT_KEY_REPEAT_INITIAL_DELAY = 0.3F; // Seconds before first repeat
        constexpr float DEFAULT_KEY_REPEAT_RATE = 0.05F;         // Seconds between repeats
    } // namespace

    CursorController::CursorController()
        : m_key_repeat_initial_delay(DEFAULT_KEY_REPEAT_INITIAL_DELAY),
          m_key_repeat_rate(DEFAULT_KEY_REPEAT_RATE)
    {}

    void CursorController::update(const InputManager &input, Cursor &cursor,
                                  const Vector2i &grid_size, float delta_time)
    {
        std::array<KeyState *, 4> key_states = {&m_key_up, &m_key_down, &m_key_left, &m_key_right};

        for (auto *p_key_state : key_states)
        {
            p_key_state->just_pressed = input.is_key_just_pressed(p_key_state->scancode);
            p_key_state->pressed = input.is_key_pressed(p_key_state->scancode);
            p_key_state->just_released = input.is_key_just_released(p_key_state->scancode);

            if (p_key_state->just_released)
            {
                p_key_state->repeat_timer = 0.0F;
            }
        }

        const bool any_key_just_pressed = m_key_up.just_pressed || m_key_down.just_pressed ||
                                          m_key_left.just_pressed || m_key_right.just_pressed;
        // Handle initial press
        if (any_key_just_pressed)
        {
            for (auto *p_key_state : key_states)
            {
                if (p_key_state->just_pressed)
                {
                    log_info("Key " + std::to_string(p_key_state->scancode) + " just pressed");

                    p_key_state->on_press(cursor);
                    p_key_state->repeat_timer = m_key_repeat_initial_delay;
                }
            }

            cursor.clamp_to_grid(grid_size);
            return;
        }

        // Handle key repeat
        for (auto *p_key_state : key_states)
        {
            if (p_key_state->pressed)
            {
                p_key_state->repeat_timer -= delta_time;

                if (p_key_state->repeat_timer <= 0.0F)
                {
                    log_info("Key " + std::to_string(p_key_state->scancode) + " repeated");

                    p_key_state->on_press(cursor);
                    p_key_state->repeat_timer = m_key_repeat_rate;
                }
            }
        }
        cursor.clamp_to_grid(grid_size);
    }
} // namespace Tactics
