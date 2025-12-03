#pragma once

#include "Tactics/Core/Camera.hpp"
#include "Tactics/Core/Vector2.hpp"
#include <SDL3/SDL.h>

namespace Tactics
{
    class Cursor
    {
    public:
        Cursor();
        explicit Cursor(const Vector2i &position);
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

        // Move cursor by offset (in tiles)
        void move(const Vector2i &offset);

        // Move cursor in a direction (1 tile)
        void move_up();
        void move_down();
        void move_left();
        void move_right();

        // Clamp cursor position to grid bounds
        void clamp_to_grid(const Vector2i &grid_size);

        // Render the cursor
        auto render(SDL_Renderer *renderer, const Camera &camera, float tile_size) const -> bool;

    private:
        Vector2i m_position;
    };
} // namespace Tactics
