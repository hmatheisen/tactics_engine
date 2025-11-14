#include <Tactics/Core/Grid.hpp>
#include <Tactics/Core/Logger.hpp>

namespace Tactics
{
    Grid::Grid() : m_width(0), m_height(0) {}

    Grid::Grid(int width, int height) : m_width(width), m_height(height)
    {
        initialize(width, height);
    }

    auto Grid::get_width() const -> int
    {
        return m_width;
    }

    auto Grid::get_height() const -> int
    {
        return m_height;
    }

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
        log_debug("Tile set at position (" + std::to_string(position.x) + ", " +
                  std::to_string(position.y) +
                  "), type: " + std::to_string(static_cast<int>(tile.get_type())) +
                  ", move_cost: " + std::to_string(tile.get_move_cost()));
    }

    void Grid::initialize(int width, int height)
    {
        log_info("Initializing grid: " + std::to_string(width) + "x" + std::to_string(height));
        m_width = width;
        m_height = height;
        m_tiles.clear();
        m_tiles.resize(static_cast<size_t>(width) * static_cast<size_t>(height));

        // Initialize all tiles with default values
        for (int y_pos = 0; y_pos < height; ++y_pos)
        {
            for (int x_pos = 0; x_pos < width; ++x_pos)
            {
                Tile tile(Vector2i(x_pos, y_pos), TileType::Grass, 1);
                m_tiles[index_of(x_pos, y_pos)] = tile;
            }
        }
        log_debug("Grid initialized with " + std::to_string(m_tiles.size()) + " tiles");
    }

    auto Grid::is_valid_position(const Vector2i &position) const -> bool
    {
        return position.x >= 0 && position.x < m_width && position.y >= 0 && position.y < m_height;
    }

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    auto Grid::render(SDL_Renderer *renderer) const -> bool
    {
        if (renderer == nullptr)
        {
            return false;
        }

        // Grid rendering will be implemented when we have a visual representation
        // For now, this is a placeholder
        (void)renderer; // Suppress unused parameter warning
        return true;
    }

    auto Grid::index_of(int x_pos, int y_pos) const -> size_t
    {
        return (static_cast<size_t>(y_pos) * static_cast<size_t>(m_width)) +
               static_cast<size_t>(x_pos);
    }
} // namespace Tactics
