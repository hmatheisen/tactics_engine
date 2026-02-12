#include "Tactics/Renderers/UnitRenderer.hpp"

#include "SDL3/SDL_stdinc.h"
#include "Tactics/Core/Rect.hpp"

#include <algorithm>

namespace Tactics
{
    namespace
    {
        constexpr uint8_t UNIT_COLOR_R = 200;
        constexpr uint8_t UNIT_COLOR_G = 40;
        constexpr uint8_t UNIT_COLOR_B = 40;
        constexpr uint8_t UNIT_BORDER_R = 20;
        constexpr uint8_t UNIT_BORDER_G = 20;
        constexpr uint8_t UNIT_BORDER_B = 20;
    } // namespace

    void UnitRenderer::render_units(SDL_Renderer *renderer, const Camera &camera, float tile_size,
                                    const std::vector<Unit> &units)
    {
        if (renderer == nullptr)
        {
            return;
        }

        for (const auto &unit : units)
        {
            const Vector2i position = unit.get_position();

            const float world_x = static_cast<float>(position.x) * tile_size;
            const float world_y = static_cast<float>(position.y) * tile_size;

            const Vector2f screen_pos = camera.world_to_screen({world_x, world_y});
            const float screen_tile_size = tile_size * camera.get_zoom();

            const float inset = screen_tile_size * 0.15F;
            const float size = std::max(0.0F, screen_tile_size - (inset * 2.0F));
            const Rectf screen_rect(screen_pos.x - (size * 0.5F), screen_pos.y - (size * 0.5F),
                                    size, size);

            if (screen_rect.right() < 0.0F || screen_rect.left() > camera.get_viewport_width() ||
                screen_rect.bottom() < 0.0F || screen_rect.top() > camera.get_viewport_height())
            {
                continue;
            }

            SDL_SetRenderDrawColor(renderer, UNIT_COLOR_R, UNIT_COLOR_G, UNIT_COLOR_B,
                                   SDL_MAX_UINT8);
            SDL_FRect sdl_rect = {screen_rect.x, screen_rect.y, screen_rect.width,
                                  screen_rect.height};
            SDL_RenderFillRect(renderer, &sdl_rect);

            SDL_SetRenderDrawColor(renderer, UNIT_BORDER_R, UNIT_BORDER_G, UNIT_BORDER_B,
                                   SDL_MAX_UINT8);
            SDL_RenderRect(renderer, &sdl_rect);
        }
    }
} // namespace Tactics
