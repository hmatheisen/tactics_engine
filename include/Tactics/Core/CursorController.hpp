#pragma once

#include "Tactics/Core/CursorEvents.hpp"
#include <SDL3/SDL.h>
#include <Tactics/Core/Cursor.hpp>
#include <Tactics/Core/Vector2.hpp>
#include <cstddef>

namespace Tactics
{
    struct KeyState
    {
        bool pressed = false;
        bool just_pressed = false;
        bool just_released = false;

        SDL_Scancode scancode;
    };

    class CursorController : public Publisher
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
        void update(Cursor &cursor, const Vector2i &grid_size, float delta_time);

    private:
        const float m_key_repeat_initial_delay;
        const float m_key_repeat_rate;

        float m_movement_repeat_timer = 0.0F;

        KeyState m_key_up = {.scancode = SDL_SCANCODE_W};
        KeyState m_key_down = {.scancode = SDL_SCANCODE_S};
        KeyState m_key_left = {.scancode = SDL_SCANCODE_A};
        KeyState m_key_right = {.scancode = SDL_SCANCODE_D};
    };
} // namespace Tactics
