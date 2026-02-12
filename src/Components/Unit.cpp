#include "Tactics/Components/Unit.hpp"

#include <algorithm>

namespace Tactics
{
    Unit::Unit() = default;

    Unit::Unit(const Vector2i &position, int move_points)
        : m_position(position), m_move_points(move_points)
    {}

    auto Unit::get_position() const -> Vector2i
    {
        return m_position;
    }

    void Unit::set_position(const Vector2i &position)
    {
        m_position = position;
    }

    auto Unit::get_move_points() const -> int
    {
        return m_move_points;
    }

    void Unit::set_move_points(int move_points)
    {
        m_move_points = std::max(0, move_points);
    }
} // namespace Tactics
