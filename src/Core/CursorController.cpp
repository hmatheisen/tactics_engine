#include <Tactics/Core/CursorController.hpp>

namespace Tactics
{
    namespace
    {
        constexpr float DEFAULT_KEY_REPEAT_INITIAL_DELAY = 0.3F; // Seconds before first repeat
        constexpr float DEFAULT_KEY_REPEAT_RATE = 0.05F;         // Seconds between repeats
    } // namespace

    CursorController::CursorController()
        : m_key_repeat_timer(0.0F), m_any_movement_key_was_pressed(false),
          m_key_repeat_initial_delay(DEFAULT_KEY_REPEAT_INITIAL_DELAY),
          m_key_repeat_rate(DEFAULT_KEY_REPEAT_RATE)
    {}

    void CursorController::update(InputManager &input, Cursor &cursor, const Vector2i &grid_size,
                                  float delta_time)
    {
        // Handle input for cursor movement with key repeat
        const bool key_up =
            input.is_key_pressed(SDL_SCANCODE_W) || input.is_key_pressed(SDL_SCANCODE_UP);
        const bool key_down =
            input.is_key_pressed(SDL_SCANCODE_S) || input.is_key_pressed(SDL_SCANCODE_DOWN);
        const bool key_left =
            input.is_key_pressed(SDL_SCANCODE_A) || input.is_key_pressed(SDL_SCANCODE_LEFT);
        const bool key_right =
            input.is_key_pressed(SDL_SCANCODE_D) || input.is_key_pressed(SDL_SCANCODE_RIGHT);

        const bool any_movement_key_pressed = key_up || key_down || key_left || key_right;

        // Update key repeat timer and return if no movement key was pressed
        if (!any_movement_key_pressed)
        {
            m_any_movement_key_was_pressed = false;
            m_key_repeat_timer = 0.0F;

            return;
        }

        if (!m_any_movement_key_was_pressed || m_key_repeat_timer <= 0.0F)
        {
            // Process movement based on which keys are pressed
            if (key_up)
            {
                cursor.move_up();
            }
            if (key_down)
            {
                cursor.move_down();
            }
            if (key_left)
            {
                cursor.move_left();
            }
            if (key_right)
            {
                cursor.move_right();
            }

            cursor.clamp_to_grid(grid_size);

            // Reset timer
            m_key_repeat_timer =
                m_any_movement_key_was_pressed ? m_key_repeat_rate : m_key_repeat_initial_delay;
            m_any_movement_key_was_pressed = true;
        }
        else
        {
            m_key_repeat_timer -= delta_time;
        }
    }

    void CursorController::set_key_repeat_initial_delay(float initial_delay)
    {
        m_key_repeat_initial_delay = initial_delay;
    }

    void CursorController::set_key_repeat_rate(float repeat_rate)
    {
        m_key_repeat_rate = repeat_rate;
    }

    auto CursorController::get_key_repeat_initial_delay() const -> float
    {
        return m_key_repeat_initial_delay;
    }

    auto CursorController::get_key_repeat_rate() const -> float
    {
        return m_key_repeat_rate;
    }
} // namespace Tactics
