#include "Tactics/Components/Grid.hpp"
#include "Tactics/Components/Tile.hpp"
#include "Tactics/Core/Logger.hpp"

namespace Tactics
{
    Grid::Grid() = default;

    auto Grid::get_width() const -> int
    {
        return m_width;
    }

    auto Grid::get_height() const -> int
    {
        return m_height;
    }

    // NOLINTNEXTLINE(readability-make-member-function-const)
    auto Grid::get_tile(const Vector2i &position) -> Tile *
    {
        if (!is_valid_position(position))
        {
            log_warning("Attempted to get tile at invalid position: (" +
                        std::to_string(position.x) + ", " + std::to_string(position.y) + ")");
            return nullptr;
        }

        return &m_tiles[index_of(position.x, position.y)];
    }

    auto Grid::get_tile(const Vector2i &position) const -> const Tile *
    {
        if (!is_valid_position(position))
        {
            return nullptr;
        }

        return &m_tiles[index_of(position.x, position.y)];
    }

    // NOLINTNEXTLINE(readability-make-member-function-const)
    void Grid::set_tile(const Vector2i &position, const Tile &tile)
    {
        if (!is_valid_position(position))
        {
            log_warning("Attempted to set tile at invalid position: (" +
                        std::to_string(position.x) + ", " + std::to_string(position.y) + ")");
            return;
        }

        m_tiles[index_of(position.x, position.y)] = tile;
        m_tiles[index_of(position.x, position.y)].set_position(position);
    }

    void Grid::resize(int width, int height)
    {
        if (width < 0 || height < 0)
        {
            log_warning("Attempted to resize grid to invalid dimensions: " + std::to_string(width) +
                        "x" + std::to_string(height));
            return;
        }

        m_width = width;
        m_height = height;
        m_tiles.clear();
        m_tiles.resize(static_cast<size_t>(width) * static_cast<size_t>(height));

        // Initialize all tiles with default values
        for (int y_pos = 0; y_pos < height; ++y_pos)
        {
            for (int x_pos = 0; x_pos < width; ++x_pos)
            {
                const Vector2i position(x_pos, y_pos);
                m_tiles[index_of(x_pos, y_pos)] = Tile(position, Tile::Type::Grass, 1);
            }
        }

        log_debug("Grid resized to: " + std::to_string(width) + "x" + std::to_string(height));
    }

    auto Grid::is_valid_position(const Vector2i &position) const -> bool
    {
        return position.x >= 0 && position.x < m_width && position.y >= 0 && position.y < m_height;
    }

    auto Grid::index_of(int x_pos, int y_pos) const -> size_t
    {
        return (static_cast<size_t>(y_pos) * static_cast<size_t>(m_width)) +
               static_cast<size_t>(x_pos);
    }
} // namespace Tactics
