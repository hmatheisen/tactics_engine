#pragma once

#include "Tactics/Core/Vector2.hpp"
#include <cstdint>

namespace Tactics
{
    class Tile
    {
    public:
        enum class Type : std::uint8_t
        {
            Grass,
            Water,
            Mountain,
            Forest,
            Desert,
            Road,
            Wall
        };

        Tile();
        Tile(Vector2i position, Type type, int move_cost);

        // Position accessors
        [[nodiscard]] auto get_position() const -> Vector2i;
        void set_position(const Vector2i &position);

        // Type accessors
        [[nodiscard]] auto get_type() const -> Type;
        void set_type(Type type);

        // Move cost accessors
        [[nodiscard]] auto get_move_cost() const -> int;
        void set_move_cost(int cost);

        // Check if tile is walkable (move_cost >= 0)
        [[nodiscard]] auto is_walkable() const -> bool;

    private:
        Vector2i m_position{0, 0};
        Type m_type{Type::Grass};
        int m_move_cost{1};
    };
} // namespace Tactics
