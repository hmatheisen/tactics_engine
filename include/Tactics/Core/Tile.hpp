#pragma once

#include "Tactics/Core/Vector2.hpp"
#include <cstdint>

namespace Tactics
{
    enum class TileType : std::uint8_t
    {
        Grass,
        Water,
        Mountain,
        Forest,
        Desert,
        Road,
        Wall
    };

    class Tile
    {
    public:
        Tile();
        Tile(Vector2i position, TileType type, int move_cost);

        // Position accessors
        [[nodiscard]] auto get_position() const -> Vector2i;
        void set_position(const Vector2i &position);

        // Type accessors
        [[nodiscard]] auto get_type() const -> TileType;
        void set_type(TileType type);

        // Move cost accessors
        [[nodiscard]] auto get_move_cost() const -> int;
        void set_move_cost(int cost);

        // Check if tile is walkable (move_cost >= 0)
        [[nodiscard]] auto is_walkable() const -> bool;

    private:
        Vector2i m_position;
        TileType m_type;
        int m_move_cost;
    };
} // namespace Tactics
