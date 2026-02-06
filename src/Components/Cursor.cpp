#include "Tactics/Components/Cursor.hpp"
#include "Tactics/Core/Rect.hpp"
#include <algorithm>

namespace Tactics
{
    namespace
    {
        constexpr uint8_t CURSOR_COLOR_R = 255;
        constexpr uint8_t CURSOR_COLOR_G = 255;
        constexpr uint8_t CURSOR_COLOR_B = 255;
        constexpr uint8_t CURSOR_COLOR_A = 128;
    } // namespace

    Cursor::Cursor() = default;

    Cursor::Cursor(const Vector2i &position) : m_position(position) {}

    auto Cursor::get_position() const -> Vector2i
    {
        return m_position;
    }

    void Cursor::set_position(const Vector2i &position)
    {
        m_position = position;
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

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    auto Cursor::render(SDL_Renderer *renderer, const Camera &camera, float tile_size) const -> bool
    {
        if (renderer == nullptr)
        {
            return false;
        }

        // Calculate world position of cursor tile
        float world_x = 0.0F;
        float world_y = 0.0F;

        world_x = static_cast<float>(m_position.x) * tile_size;
        world_y = static_cast<float>(m_position.y) * tile_size;

        // Convert to screen coordinates
        const Vector2f screen_pos = camera.world_to_screen({world_x, world_y});

        // Calculate screen size (tile size in screen space)
        const float screen_tile_size = tile_size * camera.get_zoom();

        // Create screen rectangle
        const Rectf screen_rect(screen_pos.x - (screen_tile_size * 0.5F),
                                screen_pos.y - (screen_tile_size * 0.5F), screen_tile_size,
                                screen_tile_size);

        // Only render if on screen
        if (screen_rect.right() < 0.0F || screen_rect.left() > camera.get_viewport_width() ||
            screen_rect.bottom() < 0.0F || screen_rect.top() > camera.get_viewport_height())
        {
            return true; // Not visible, but not an error
        }

        // Draw cursor highlight
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, CURSOR_COLOR_R, CURSOR_COLOR_G, CURSOR_COLOR_B,
                               CURSOR_COLOR_A);
        SDL_FRect sdl_rect = {screen_rect.x, screen_rect.y, screen_rect.width, screen_rect.height};
        SDL_RenderFillRect(renderer, &sdl_rect);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

        return true;
    }

} // namespace Tactics
