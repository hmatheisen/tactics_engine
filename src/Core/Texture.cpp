#include "Tactics/Core/Texture.hpp"
#include "Tactics/Core/Logger.hpp"
#include <SDL3/SDL_surface.h>

namespace Tactics
{

    Texture::Texture() : m_texture(nullptr) {}

    Texture::~Texture()
    {
        release();
    }

    Texture::Texture(Texture &&other) noexcept : m_texture(other.m_texture)
    {
        other.m_texture = nullptr;
    }

    auto Texture::operator=(Texture &&other) noexcept -> Texture &
    {
        if (this != &other)
        {
            release();
            m_texture = other.m_texture;
            other.m_texture = nullptr;
        }

        return *this;
    }

    auto Texture::create(SDL_Renderer *renderer, SDL_PixelFormat format, SDL_TextureAccess access,
                         int width, int height) -> Texture
    {
        Texture texture;
        texture.m_texture = SDL_CreateTexture(renderer, format, access, width, height);
        if (texture.m_texture == nullptr)
        {
            log_error("Failed to create texture: " + std::string(SDL_GetError()));
        }
        else
        {
            log_debug("Texture created: " + std::to_string(width) + "x" + std::to_string(height));
        }
        return texture;
    }

    auto Texture::create_from_surface(SDL_Renderer *renderer, SDL_Surface *surface) -> Texture
    {
        Texture texture;
        texture.m_texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (texture.m_texture == nullptr)
        {
            log_error("Failed to create texture from surface: " + std::string(SDL_GetError()));
        }
        else
        {
            log_debug("Texture created from surface");
        }
        return texture;
    }

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    auto Texture::load_from_file(SDL_Renderer *renderer, const std::string &file_path) -> Texture
    {
        log_info("Loading texture from file: " + file_path);
        Texture texture;

        SDL_Surface *surface = SDL_LoadBMP(file_path.c_str());
        if (surface == nullptr)
        {
            log_error("Failed to load BMP file: " + file_path + " - " +
                      std::string(SDL_GetError()));
            return texture; // Return invalid texture
        }

        texture.m_texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface);

        if (texture.m_texture == nullptr)
        {
            log_error("Failed to create texture from loaded surface: " +
                      std::string(SDL_GetError()));
        }
        else
        {
            log_info("Texture loaded successfully: " + file_path);
        }

        return texture;
    }

    auto Texture::is_valid() const -> bool
    {
        return m_texture != nullptr;
    }

    auto Texture::get() const -> SDL_Texture *
    {
        return m_texture;
    }

    auto Texture::get_size() const -> Vector2f
    {
        float width = 0.0F;
        float height = 0.0F;

        if (m_texture != nullptr)
        {
            SDL_GetTextureSize(m_texture, &width, &height);
        }

        return {width, height};
    }

    auto Texture::get_width() const -> float
    {
        return get_size().x;
    }

    auto Texture::get_height() const -> float
    {
        return get_size().y;
    }

    auto Texture::set_color_mod(Uint8 red, Uint8 green, Uint8 blue) -> bool
    {
        if (m_texture == nullptr)
        {
            return false;
        }

        return SDL_SetTextureColorMod(m_texture, red, green, blue);
    }

    auto Texture::set_color_mod(float red, float green, float blue) -> bool
    {
        if (m_texture == nullptr)
        {
            return false;
        }

        return SDL_SetTextureColorModFloat(m_texture, red, green, blue);
    }

    auto Texture::get_color_mod(Uint8 *red, Uint8 *green, Uint8 *blue) const -> bool
    {
        if (m_texture == nullptr)
        {
            return false;
        }

        return SDL_GetTextureColorMod(m_texture, red, green, blue);
    }

    auto Texture::get_color_mod(float *red, float *green, float *blue) const -> bool
    {
        if (m_texture == nullptr)
        {
            return false;
        }

        return SDL_GetTextureColorModFloat(m_texture, red, green, blue);
    }

    auto Texture::set_alpha_mod(Uint8 alpha) -> bool
    {
        if (m_texture == nullptr)
        {
            return false;
        }

        return SDL_SetTextureAlphaMod(m_texture, alpha);
    }

    auto Texture::set_alpha_mod(float alpha) -> bool
    {
        if (m_texture == nullptr)
        {
            return false;
        }

        return SDL_SetTextureAlphaModFloat(m_texture, alpha);
    }

    auto Texture::get_alpha_mod(Uint8 *alpha) const -> bool
    {
        if (m_texture == nullptr)
        {
            return false;
        }

        return SDL_GetTextureAlphaMod(m_texture, alpha);
    }

    auto Texture::get_alpha_mod(float *alpha) const -> bool
    {
        if (m_texture == nullptr)
        {
            return false;
        }

        return SDL_GetTextureAlphaModFloat(m_texture, alpha);
    }

    auto Texture::set_blend_mode(SDL_BlendMode blend_mode) -> bool
    {
        if (m_texture == nullptr)
        {
            return false;
        }

        return SDL_SetTextureBlendMode(m_texture, blend_mode);
    }

    auto Texture::get_blend_mode(SDL_BlendMode *blend_mode) const -> bool
    {
        if (m_texture == nullptr)
        {
            return false;
        }

        return SDL_GetTextureBlendMode(m_texture, blend_mode);
    }

    auto Texture::set_scale_mode(SDL_ScaleMode scale_mode) -> bool
    {
        if (m_texture == nullptr)
        {
            return false;
        }

        return SDL_SetTextureScaleMode(m_texture, scale_mode);
    }

    auto Texture::get_scale_mode(SDL_ScaleMode *scale_mode) const -> bool
    {
        if (m_texture == nullptr)
        {
            return false;
        }

        return SDL_GetTextureScaleMode(m_texture, scale_mode);
    }

    auto Texture::update(const SDL_Rect *rect, const void *pixels, int pitch) -> bool
    {
        if (m_texture == nullptr)
        {
            return false;
        }

        return SDL_UpdateTexture(m_texture, rect, pixels, pitch);
    }

    auto Texture::lock(const SDL_Rect *rect, void **pixels, int *pitch) -> bool
    {
        if (m_texture == nullptr)
        {
            return false;
        }

        return SDL_LockTexture(m_texture, rect, pixels, pitch);
    }

    void Texture::unlock()
    {
        if (m_texture != nullptr)
        {
            SDL_UnlockTexture(m_texture);
        }
    }

    void Texture::release()
    {
        if (m_texture != nullptr)
        {
            log_debug("Releasing texture");
            SDL_DestroyTexture(m_texture);
            m_texture = nullptr;
        }
    }

    auto Texture::render(SDL_Renderer *renderer) const -> bool
    {
        if (m_texture == nullptr || renderer == nullptr)
        {
            return false;
        }
        return SDL_RenderTexture(renderer, m_texture, nullptr, nullptr);
    }

    auto Texture::render(SDL_Renderer *renderer, const Rectf &dst_rect) const -> bool
    {
        if (m_texture == nullptr || renderer == nullptr)
        {
            return false;
        }
        SDL_FRect sdl_rect = {dst_rect.x, dst_rect.y, dst_rect.width, dst_rect.height};
        return SDL_RenderTexture(renderer, m_texture, nullptr, &sdl_rect);
    }

    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    auto Texture::render(SDL_Renderer *renderer, const Rectf &src_rect, const Rectf &dst_rect) const
        -> bool
    {
        if (m_texture == nullptr || renderer == nullptr)
        {
            return false;
        }
        SDL_FRect sdl_src = {src_rect.x, src_rect.y, src_rect.width, src_rect.height};
        SDL_FRect sdl_dst = {dst_rect.x, dst_rect.y, dst_rect.width, dst_rect.height};
        return SDL_RenderTexture(renderer, m_texture, &sdl_src, &sdl_dst);
    }

    auto Texture::render(SDL_Renderer *renderer, float x_pos, float y_pos) const -> bool
    {
        if (m_texture == nullptr || renderer == nullptr)
        {
            return false;
        }
        Vector2f size = get_size();
        SDL_FRect dst_rect = {x_pos, y_pos, size.x, size.y};
        return SDL_RenderTexture(renderer, m_texture, nullptr, &dst_rect);
    }

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    auto Texture::render(SDL_Renderer *renderer, const Vector2f &position) const -> bool
    {
        return render(renderer, position.x, position.y);
    }

} // namespace Tactics
