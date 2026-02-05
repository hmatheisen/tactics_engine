#pragma once

#include "Tactics/Core/Camera.hpp"
#include "Tactics/Core/Vector2.hpp"
#include <SDL3/SDL.h>

namespace Tactics
{
    class Unit
    {
    public:
        Unit();
        Unit(const Vector2i &position, int move_points);
        ~Unit() = default;

        // Delete copy constructor and assignment operator
        Unit(const Unit &) = delete;
        auto operator=(const Unit &) -> Unit & = delete;

        // Move constructor
        Unit(Unit &&other) noexcept = default;

        // Move assignment operator
        auto operator=(Unit &&other) noexcept -> Unit & = default;

        [[nodiscard]] auto get_position() const -> Vector2i;
        void set_position(const Vector2i &position);

        [[nodiscard]] auto get_move_points() const -> int;
        void set_move_points(int move_points);

        auto render(SDL_Renderer *renderer, const Camera &camera, float tile_size) const -> bool;

    private:
        Vector2i m_position{0, 0};
        int m_move_points = 0;
    };
} // namespace Tactics
