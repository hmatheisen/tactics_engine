#include "Tactics/Components/Cursor.hpp"

#include <algorithm>

namespace Tactics
{
    Cursor::Cursor(float tile_size) : m_tile_size(tile_size) {}

    auto Cursor::get_position() const -> Vector2i
    {
        return m_position;
    }

    void Cursor::set_position(const Vector2i &position)
    {
        m_position = position;
    }

    auto Cursor::get_world_position() const -> Vector2f
    {
        const float world_x = static_cast<float>(m_position.x) * m_tile_size;
        const float world_y = static_cast<float>(m_position.y) * m_tile_size;

        return {world_x, world_y};
    }

    void Cursor::set_world_position(const Vector2f &world_position)
    {
        m_position.x = static_cast<int>(std::round(world_position.x / m_tile_size));
        m_position.y = static_cast<int>(std::round(world_position.y / m_tile_size));
    }

    void Cursor::move(const Vector2i &offset)
    {
        m_position.x += offset.x;
        m_position.y -= offset.y;
    }

    void Cursor::move_up()
    {
        m_position.y -= 1;
    }

    void Cursor::move_down()
    {
        m_position.y += 1;
    }

    void Cursor::move_left()
    {
        m_position.x -= 1;
    }

    void Cursor::move_right()
    {
        m_position.x += 1;
    }

    void Cursor::clamp_to_grid(const Vector2i &grid_size)
    {
        m_position.x = std::clamp(m_position.x, 0, grid_size.x - 1);
        m_position.y = std::clamp(m_position.y, 0, grid_size.y - 1);
    }

} // namespace Tactics
