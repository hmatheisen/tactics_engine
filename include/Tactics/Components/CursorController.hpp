#pragma once

#include "SDL3/SDL_scancode.h"
#include "Tactics/Components/Cursor.hpp"
#include "Tactics/Core/EventBus.hpp"
#include "Tactics/Core/Vector2.hpp"
#include <SDL3/SDL.h>
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
        CursorController() = default;
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
        static constexpr float DEFAULT_KEY_REPEAT_INITIAL_DELAY = 0.2F;
        static constexpr float DEFAULT_KEY_REPEAT_RATE = 0.04F;

        const float m_key_repeat_initial_delay{DEFAULT_KEY_REPEAT_INITIAL_DELAY};
        const float m_key_repeat_rate{DEFAULT_KEY_REPEAT_RATE};

        float m_movement_repeat_timer = 0.0F;

        KeyState m_key_up = {.scancode = SDL_SCANCODE_W};
        KeyState m_key_down = {.scancode = SDL_SCANCODE_S};
        KeyState m_key_left = {.scancode = SDL_SCANCODE_A};
        KeyState m_key_right = {.scancode = SDL_SCANCODE_D};

        KeyState m_key_select = {.scancode = SDL_SCANCODE_LSHIFT};

        void handle_pressed(Cursor &cursor, const Vector2i &grid_size) const;
        void handle_just_pressed(Cursor &cursor, const Vector2i &grid_size) const;
    };
} // namespace Tactics
