#pragma once

#include <SDL3/SDL.h>
#include <Tactics/Core/Vector2.hpp>
#include <array>
#include <cstdint>

namespace Tactics
{
    class InputManager
    {
    public:
        InputManager();
        ~InputManager() = default;

        // Delete copy constructor and assignment operator
        InputManager(const InputManager &) = delete;
        auto operator=(const InputManager &) -> InputManager & = delete;

        // Delete move constructor and assignment operator
        InputManager(InputManager &&) = delete;
        auto operator=(InputManager &&) -> InputManager & = delete;

        // Update input state (call at the start of each frame)
        void update();

        // Process an SDL event
        void process_event(const SDL_Event &event);

        // Keyboard state queries
        [[nodiscard]] auto is_key_pressed(SDL_Scancode scancode) const -> bool;
        [[nodiscard]] auto is_key_just_pressed(SDL_Scancode scancode) const -> bool;
        [[nodiscard]] auto is_key_just_released(SDL_Scancode scancode) const -> bool;

        // Mouse state queries
        [[nodiscard]] auto get_mouse_position() const -> Vector2f;
        [[nodiscard]] auto get_mouse_position_previous() const -> Vector2f;
        [[nodiscard]] auto get_mouse_delta() const -> Vector2f;

        [[nodiscard]] auto is_mouse_button_pressed(std::uint32_t button) const -> bool;
        [[nodiscard]] auto is_mouse_button_just_pressed(std::uint32_t button) const -> bool;
        [[nodiscard]] auto is_mouse_button_just_released(std::uint32_t button) const -> bool;

        [[nodiscard]] auto get_mouse_wheel_delta() const -> Vector2f;
        [[nodiscard]] auto get_mouse_wheel_delta_previous() const -> Vector2f;

        // Mouse button mask constants (for use with button state queries)
        static constexpr std::uint32_t MOUSE_BUTTON_LEFT = SDL_BUTTON_LMASK;
        static constexpr std::uint32_t MOUSE_BUTTON_MIDDLE = SDL_BUTTON_MMASK;
        static constexpr std::uint32_t MOUSE_BUTTON_RIGHT = SDL_BUTTON_RMASK;
        static constexpr std::uint32_t MOUSE_BUTTON_X1 = SDL_BUTTON_X1MASK;
        static constexpr std::uint32_t MOUSE_BUTTON_X2 = SDL_BUTTON_X2MASK;

    private:
        // Maximum number of scancodes (SDL_NUM_SCANCODES is typically 512)
        static constexpr int MAX_SCANCODES = 512;

        // Keyboard state
        std::array<bool, MAX_SCANCODES> m_current_keys;
        std::array<bool, MAX_SCANCODES> m_previous_keys;

        // Mouse state
        Vector2f m_mouse_position;
        Vector2f m_previous_mouse_position;
        std::uint32_t m_current_mouse_buttons;
        std::uint32_t m_previous_mouse_buttons;
        Vector2f m_mouse_wheel_delta;
        Vector2f m_previous_mouse_wheel_delta;
    };
} // namespace Tactics
