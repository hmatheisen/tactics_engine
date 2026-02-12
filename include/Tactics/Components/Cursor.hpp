#pragma once

#include "Tactics/Core/Vector2.hpp"

namespace Tactics
{
    class Cursor
    {
    public:
        explicit Cursor(float tile_size);
        ~Cursor() = default;

        // Delete copy constructor and assignment operator
        Cursor(const Cursor &) = delete;
        auto operator=(const Cursor &) -> Cursor & = delete;

        // Move constructor
        Cursor(Cursor &&other) noexcept = default;

        // Move assignment operator
        auto operator=(Cursor &&other) noexcept -> Cursor & = default;

        // Position accessors (in tile coordinates)
        [[nodiscard]] auto get_position() const -> Vector2i;
        void set_position(const Vector2i &position);

        // World-space helpers (derived from tile coordinates)
        [[nodiscard]] auto get_world_position() const -> Vector2f;
        void set_world_position(const Vector2f &world_position);

        // Move cursor by offset (in tiles)
        void move(const Vector2i &offset);

        // Move cursor in a direction (1 tile)
        void move_up();
        void move_down();
        void move_left();
        void move_right();

        // Clamp cursor position to grid bounds
        void clamp_to_grid(const Vector2i &grid_size);

    private:
        Vector2i m_position{0, 0};
        float m_tile_size;
    };
} // namespace Tactics
