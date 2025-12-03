#include "Tactics/Core/Tile.hpp"

namespace Tactics
{
    Tile::Tile() : m_position(0, 0), m_type(Tile::Type::Grass), m_move_cost(1) {}

    Tile::Tile(Vector2i position, Tile::Type type, int move_cost)
        : m_position(position), m_type(type), m_move_cost(move_cost)
    {}

    auto Tile::get_position() const -> Vector2i
    {
        return m_position;
    }

    void Tile::set_position(const Vector2i &position)
    {
        m_position = position;
    }

    auto Tile::get_type() const -> Tile::Type
    {
        return m_type;
    }

    void Tile::set_type(Tile::Type type)
    {
        m_type = type;
    }

    auto Tile::get_move_cost() const -> int
    {
        return m_move_cost;
    }

    void Tile::set_move_cost(int cost)
    {
        m_move_cost = cost;
    }

    auto Tile::is_walkable() const -> bool
    {
        return m_move_cost >= 0;
    }
} // namespace Tactics
