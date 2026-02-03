#include "Tactics/Core/Grid.hpp"
#include "SDL3/SDL_stdinc.h"
#include "Tactics/Core/Logger.hpp"
#include "Tactics/Core/Tile.hpp"
#include <cmath>

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
        log_debug("Tile set at position (" + std::to_string(position.x) + ", " +
                  std::to_string(position.y) +
                  "), type: " + std::to_string(static_cast<int>(tile.get_type())) +
                  ", move_cost: " + std::to_string(tile.get_move_cost()));
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

    namespace
    {
        // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        auto tile_type_to_color(Tile::Type type) -> SDL_Color
        {
            switch (type)
            {
            case Tile::Type::Grass:
                return {34, 139, 34, 255}; // Green
            case Tile::Type::Water:
                return {0, 119, 190, 255}; // Blue
            case Tile::Type::Mountain:
                return {139, 137, 137, 255}; // Gray
            case Tile::Type::Forest:
                return {34, 100, 34, 255}; // Dark green
            case Tile::Type::Desert:
                return {238, 203, 173, 255}; // Beige
            case Tile::Type::Road:
                return {105, 105, 105, 255}; // Dark gray
            case Tile::Type::Wall:
                return {64, 64, 64, 255}; // Very dark gray
            default:
                return {128, 128, 128, 255}; // Gray
            }
        }
        // NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

    } // namespace

    auto Grid::render(SDL_Renderer *renderer, const Camera &camera) const -> bool
    {
        if (renderer == nullptr)
        {
            return false;
        }

        constexpr float TILE_SIZE = 32.0F; // Size of each tile in world units

        // Get the camera's view rectangle to only render visible tiles
        const Rectf view_rect = camera.get_view_rect();

        // Calculate which tiles are visible
        const int start_x = static_cast<int>((view_rect.left() - TILE_SIZE) / TILE_SIZE);
        const int end_x = static_cast<int>((view_rect.right() + TILE_SIZE) / TILE_SIZE);
        const int start_y = static_cast<int>((view_rect.top() - TILE_SIZE) / TILE_SIZE);
        const int end_y = static_cast<int>((view_rect.bottom() + TILE_SIZE) / TILE_SIZE);

        // Render visible tiles
        for (int y_pos = start_y; y_pos <= end_y; ++y_pos)
        {
            for (int x_pos = start_x; x_pos <= end_x; ++x_pos)
            {
                const Vector2i tile_pos(x_pos, y_pos);
                if (!is_valid_position(tile_pos))
                {
                    continue;
                }

                const Tile *tile = get_tile(tile_pos);
                if (tile == nullptr)
                {
                    continue;
                }

                // Calculate world position of tile
                const float world_x = static_cast<float>(x_pos) * TILE_SIZE;
                const float world_y = static_cast<float>(y_pos) * TILE_SIZE;

                // Convert to screen coordinates
                const Vector2f screen_pos = camera.world_to_screen({world_x, world_y});

                // Calculate screen size (tile size in screen space)
                const float screen_tile_size = TILE_SIZE * camera.get_zoom();

                // Create screen rectangle
                const Rectf screen_rect(screen_pos.x - (screen_tile_size * 0.5F),
                                        screen_pos.y - (screen_tile_size * 0.5F), screen_tile_size,
                                        screen_tile_size);

                // Only render if on screen
                if (screen_rect.right() < 0.0F ||
                    screen_rect.left() > camera.get_viewport_width() ||
                    screen_rect.bottom() < 0.0F || screen_rect.top() > camera.get_viewport_height())
                {
                    continue;
                }

                // Get color for tile type
                const SDL_Color color = tile_type_to_color(tile->get_type());

                // Draw filled rectangle
                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
                const float aligned_left = std::floor(screen_rect.left());
                const float aligned_top = std::floor(screen_rect.top());
                const float aligned_right = std::ceil(screen_rect.right());
                const float aligned_bottom = std::ceil(screen_rect.bottom());
                SDL_FRect sdl_rect = {aligned_left, aligned_top, aligned_right - aligned_left,
                                      aligned_bottom - aligned_top};
                SDL_RenderFillRect(renderer, &sdl_rect);

                // Draw border
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_MAX_UINT8);
                SDL_RenderRect(renderer, &sdl_rect);
            }
        }

        return true;
    }

    auto Grid::index_of(int x_pos, int y_pos) const -> size_t
    {
        return (static_cast<size_t>(y_pos) * static_cast<size_t>(m_width)) +
               static_cast<size_t>(x_pos);
    }
} // namespace Tactics
