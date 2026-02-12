#include "Tactics/Renderers/CursorRenderer.hpp"

#include "Tactics/Core/Rect.hpp"

namespace Tactics
{
    namespace
    {
        constexpr uint8_t CURSOR_COLOR_R = 255;
        constexpr uint8_t CURSOR_COLOR_G = 255;
        constexpr uint8_t CURSOR_COLOR_B = 255;
        constexpr uint8_t CURSOR_COLOR_A = 128;
    } // namespace

    auto CursorRenderer::render(SDL_Renderer *renderer, const Cursor &cursor, const Camera &camera,
                                float tile_size) -> bool
    {
        if (renderer == nullptr)
        {
            return false;
        }

        const Vector2i position = cursor.get_position();

        const float world_x = static_cast<float>(position.x) * tile_size;
        const float world_y = static_cast<float>(position.y) * tile_size;

        const Vector2f screen_pos = camera.world_to_screen({world_x, world_y});

        const float screen_tile_size = tile_size * camera.get_zoom();

        const Rectf screen_rect(screen_pos.x - (screen_tile_size * 0.5F),
                                screen_pos.y - (screen_tile_size * 0.5F), screen_tile_size,
                                screen_tile_size);

        if (screen_rect.right() < 0.0F || screen_rect.left() > camera.get_viewport_width() ||
            screen_rect.bottom() < 0.0F || screen_rect.top() > camera.get_viewport_height())
        {
            return true;
        }

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, CURSOR_COLOR_R, CURSOR_COLOR_G, CURSOR_COLOR_B,
                               CURSOR_COLOR_A);
        SDL_FRect sdl_rect = {screen_rect.x, screen_rect.y, screen_rect.width, screen_rect.height};
        SDL_RenderFillRect(renderer, &sdl_rect);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

        return true;
    }
} // namespace Tactics
