#include <Tactics/Core/Texture.hpp>

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
        return texture;
    }

    auto Texture::create_from_surface(SDL_Renderer *renderer, SDL_Surface *surface) -> Texture
    {
        Texture texture;
        texture.m_texture = SDL_CreateTextureFromSurface(renderer, surface);
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
            SDL_DestroyTexture(m_texture);
            m_texture = nullptr;
        }
    }

} // namespace Tactics
