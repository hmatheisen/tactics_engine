#pragma once

#include <SDL3/SDL.h>
#include <Tactics/Core/Cursor.hpp>
#include <Tactics/Core/InputManager.hpp>
#include <Tactics/Core/Vector2.hpp>
#include <functional>

namespace Tactics
{

    struct KeyState
    {
        bool pressed = false;
        bool just_pressed = false;
        bool just_released = false;
        float repeat_timer = 0.0F;
        SDL_Scancode scancode;
        std::function<void(Cursor &cursor)> on_press;
    };

    class CursorController
    {
    public:
        CursorController();
        ~CursorController() = default;

        // Delete copy constructor and assignment operator
        CursorController(const CursorController &) = delete;
        auto operator=(const CursorController &) -> CursorController & = delete;

        // Delete move constructor and assignment operator
        CursorController(CursorController &&) = delete;
        auto operator=(CursorController &&) -> CursorController & = delete;

        // Update cursor based on input
        void update(const InputManager &input, Cursor &cursor, const Vector2i &grid_size,
                    float delta_time);

    private:
        const float m_key_repeat_initial_delay;
        const float m_key_repeat_rate;

        KeyState m_key_up = {.scancode = SDL_SCANCODE_W,
                             .on_press = [](Cursor &cursor) -> void { cursor.move_up(); }};
        KeyState m_key_down = {.scancode = SDL_SCANCODE_S,
                               .on_press = [](Cursor &cursor) -> void { cursor.move_down(); }};
        KeyState m_key_left = {.scancode = SDL_SCANCODE_A,
                               .on_press = [](Cursor &cursor) -> void { cursor.move_left(); }};
        KeyState m_key_right = {.scancode = SDL_SCANCODE_D,
                                .on_press = [](Cursor &cursor) -> void { cursor.move_right(); }};
    };
} // namespace Tactics
