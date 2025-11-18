#include <Tactics/Core/InputManager.hpp>
#include <Tactics/Core/Logger.hpp>
#include <algorithm>

namespace Tactics
{
    InputManager::InputManager()
        : m_current_keys{}, m_previous_keys{}, m_mouse_position(0.0F, 0.0F),
          m_previous_mouse_position(0.0F, 0.0F), m_current_mouse_buttons(0),
          m_previous_mouse_buttons(0), m_mouse_wheel_delta(0.0F, 0.0F),
          m_previous_mouse_wheel_delta(0.0F, 0.0F)
    {
        // Initialize all keys to false
        m_current_keys.fill(false);
        m_previous_keys.fill(false);
    }

    void InputManager::update()
    {
        // Save previous frame state
        m_previous_keys = m_current_keys;
        m_previous_mouse_position = m_mouse_position;
        m_previous_mouse_buttons = m_current_mouse_buttons;
        m_previous_mouse_wheel_delta = m_mouse_wheel_delta;

        // Get current keyboard state
        int num_keys = 0;
        const bool *keyboard_state = SDL_GetKeyboardState(&num_keys);
        if (keyboard_state != nullptr)
        {
            const int keys_to_copy = std::min(num_keys, MAX_SCANCODES);
            std::copy_n(keyboard_state, keys_to_copy, m_current_keys.begin());
        }

        // Get current mouse state
        float mouse_x = 0.0F;
        float mouse_y = 0.0F;
        SDL_MouseButtonFlags mouse_buttons = SDL_GetMouseState(&mouse_x, &mouse_y);
        m_mouse_position = Vector2f(mouse_x, mouse_y);
        m_current_mouse_buttons = static_cast<std::uint32_t>(mouse_buttons);

        // Reset mouse wheel delta (it's event-based, so we reset it each frame)
        m_mouse_wheel_delta = Vector2f(0.0F, 0.0F);
    }

    void InputManager::process_event(const SDL_Event &event)
    {
        switch (event.type)
        {
        case SDL_EVENT_MOUSE_WHEEL:
        {
            // Accumulate wheel delta
            m_mouse_wheel_delta.x += event.wheel.x;
            m_mouse_wheel_delta.y += event.wheel.y;
            break;
        }
        default:
            // Other events are handled by update() via SDL_GetKeyboardState and SDL_GetMouseState
            break;
        }
    }

    auto InputManager::is_key_pressed(SDL_Scancode scancode) const -> bool
    {
        const int index = static_cast<int>(scancode);
        if (index < 0 || index >= MAX_SCANCODES)
        {
            return false;
        }

        return m_current_keys.at(static_cast<size_t>(index));
    }

    auto InputManager::is_key_just_pressed(SDL_Scancode scancode) const -> bool
    {
        const int index = static_cast<int>(scancode);
        if (index < 0 || index >= MAX_SCANCODES)
        {
            return false;
        }

        const auto idx = static_cast<size_t>(index);
        return m_current_keys.at(idx) && !m_previous_keys.at(idx);
    }

    auto InputManager::is_key_just_released(SDL_Scancode scancode) const -> bool
    {
        const int index = static_cast<int>(scancode);
        if (index < 0 || index >= MAX_SCANCODES)
        {
            return false;
        }

        const auto idx = static_cast<size_t>(index);
        return !m_current_keys.at(idx) && m_previous_keys.at(idx);
    }

    auto InputManager::get_mouse_position() const -> Vector2f
    {
        return m_mouse_position;
    }

    auto InputManager::get_mouse_position_previous() const -> Vector2f
    {
        return m_previous_mouse_position;
    }

    auto InputManager::get_mouse_delta() const -> Vector2f
    {
        return m_mouse_position - m_previous_mouse_position;
    }

    auto InputManager::is_mouse_button_pressed(std::uint32_t button) const -> bool
    {
        return (m_current_mouse_buttons & button) != 0;
    }

    auto InputManager::is_mouse_button_just_pressed(std::uint32_t button) const -> bool
    {
        return ((m_current_mouse_buttons & button) != 0) &&
               ((m_previous_mouse_buttons & button) == 0);
    }

    auto InputManager::is_mouse_button_just_released(std::uint32_t button) const -> bool
    {
        return ((m_current_mouse_buttons & button) == 0) &&
               ((m_previous_mouse_buttons & button) != 0);
    }

    auto InputManager::get_mouse_wheel_delta() const -> Vector2f
    {
        return m_mouse_wheel_delta;
    }

    auto InputManager::get_mouse_wheel_delta_previous() const -> Vector2f
    {
        return m_previous_mouse_wheel_delta;
    }

} // namespace Tactics
