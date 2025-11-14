#pragma once

#include <SDL3/SDL.h>
#include <Tactics/Core/Cursor.hpp>
#include <Tactics/Core/InputManager.hpp>
#include <Tactics/Core/Vector2.hpp>

namespace Tactics
{
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
        void update(InputManager &input, Cursor &cursor, const Vector2i &grid_size,
                    float delta_time);

        // Key repeat settings
        void set_key_repeat_initial_delay(float initial_delay);
        void set_key_repeat_rate(float repeat_rate);
        [[nodiscard]] auto get_key_repeat_initial_delay() const -> float;
        [[nodiscard]] auto get_key_repeat_rate() const -> float;

    private:
        float m_key_repeat_timer;
        bool m_any_movement_key_was_pressed;
        float m_key_repeat_initial_delay;
        float m_key_repeat_rate;
    };
} // namespace Tactics
