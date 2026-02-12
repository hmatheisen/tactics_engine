#include "Tactics/Renderers/GridRenderer.hpp"

#include "SDL3/SDL_stdinc.h"
#include "Tactics/Components/Tile.hpp"
#include "Tactics/Core/Rect.hpp"

#include <cmath>

namespace Tactics
{
    namespace
    {
        constexpr SDL_Color GRASS_COLOR = {34, 139, 34, 255};
        constexpr SDL_Color WATER_COLOR = {0, 119, 190, 255};
        constexpr SDL_Color MOUNTAIN_COLOR = {139, 137, 137, 255};
        constexpr SDL_Color FOREST_COLOR = {34, 100, 34, 255};
        constexpr SDL_Color DESERT_COLOR = {238, 203, 173, 255};
        constexpr SDL_Color ROAD_COLOR = {105, 105, 105, 255};
        constexpr SDL_Color WALL_COLOR = {64, 64, 64, 255};
        constexpr SDL_Color DEFAULT_COLOR = {128, 128, 128, 255};

        auto tile_type_to_color(Tile::Type type) -> SDL_Color
        {
            switch (type)
            {
            case Tile::Type::Grass:
                return GRASS_COLOR;
            case Tile::Type::Water:
                return WATER_COLOR;
            case Tile::Type::Mountain:
                return MOUNTAIN_COLOR;
            case Tile::Type::Forest:
                return FOREST_COLOR;
            case Tile::Type::Desert:
                return DESERT_COLOR;
            case Tile::Type::Road:
                return ROAD_COLOR;
            case Tile::Type::Wall:
                return WALL_COLOR;
            default:
                return DEFAULT_COLOR;
            }
        }
    } // namespace

    auto GridRenderer::render(SDL_Renderer *renderer, const Grid &grid, const Camera &camera,
                              float tile_size) -> bool
    {
        if (renderer == nullptr)
        {
            return false;
        }

        const Rectf view_rect = camera.get_view_rect();

        const int start_x = static_cast<int>((view_rect.left() - tile_size) / tile_size);
        const int end_x = static_cast<int>((view_rect.right() + tile_size) / tile_size);
        const int start_y = static_cast<int>((view_rect.top() - tile_size) / tile_size);
        const int end_y = static_cast<int>((view_rect.bottom() + tile_size) / tile_size);

        for (int y_pos = start_y; y_pos <= end_y; ++y_pos)
        {
            for (int x_pos = start_x; x_pos <= end_x; ++x_pos)
            {
                const Vector2i tile_pos(x_pos, y_pos);
                if (!grid.is_valid_position(tile_pos))
                {
                    continue;
                }

                const Tile *tile = grid.get_tile(tile_pos);
                if (tile == nullptr)
                {
                    continue;
                }

                const float world_x = static_cast<float>(x_pos) * tile_size;
                const float world_y = static_cast<float>(y_pos) * tile_size;

                const Vector2f screen_pos = camera.world_to_screen({world_x, world_y});

                const float screen_tile_size = tile_size * camera.get_zoom();

                const Rectf screen_rect(screen_pos.x - (screen_tile_size * 0.5F),
                                        screen_pos.y - (screen_tile_size * 0.5F), screen_tile_size,
                                        screen_tile_size);

                if (screen_rect.right() < 0.0F ||
                    screen_rect.left() > camera.get_viewport_width() ||
                    screen_rect.bottom() < 0.0F || screen_rect.top() > camera.get_viewport_height())
                {
                    continue;
                }

                const SDL_Color color = tile_type_to_color(tile->get_type());

                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
                const float aligned_left = std::floor(screen_rect.left());
                const float aligned_top = std::floor(screen_rect.top());
                const float aligned_right = std::ceil(screen_rect.right());
                const float aligned_bottom = std::ceil(screen_rect.bottom());
                SDL_FRect sdl_rect = {aligned_left, aligned_top, aligned_right - aligned_left,
                                      aligned_bottom - aligned_top};
                SDL_RenderFillRect(renderer, &sdl_rect);

                SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_MAX_UINT8);
                SDL_RenderRect(renderer, &sdl_rect);
            }
        }

        return true;
    }
} // namespace Tactics
