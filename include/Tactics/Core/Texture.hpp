#pragma once

#include <SDL3/SDL.h>
#include <Tactics/Core/Rect.hpp>
#include <Tactics/Core/Vector2.hpp>
#include <string>

namespace Tactics
{
    class Texture
    {
    public:
        Texture();
        ~Texture();

        // Delete copy constructor and assignment operator
        Texture(const Texture &) = delete;
        auto operator=(const Texture &) -> Texture & = delete;

        // Move constructor
        Texture(Texture &&other) noexcept;

        // Move assignment operator
        auto operator=(Texture &&other) noexcept -> Texture &;

        // Create texture from renderer
        static auto create(SDL_Renderer *renderer, SDL_PixelFormat format, SDL_TextureAccess access,
                           int width, int height) -> Texture;

        // Create texture from surface
        static auto create_from_surface(SDL_Renderer *renderer, SDL_Surface *surface) -> Texture;

        // Load texture from file (BMP format)
        static auto load_from_file(SDL_Renderer *renderer, const std::string &file_path) -> Texture;

        // Check if texture is valid
        [[nodiscard]] auto is_valid() const -> bool;

        // Get the underlying SDL_Texture pointer
        [[nodiscard]] auto get() const -> SDL_Texture *;

        // Get texture size
        [[nodiscard]] auto get_size() const -> Vector2f;

        // Get texture width and height
        [[nodiscard]] auto get_width() const -> float;
        [[nodiscard]] auto get_height() const -> float;

        // Color modulation
        auto set_color_mod(Uint8 red, Uint8 green, Uint8 blue) -> bool;
        auto set_color_mod(float red, float green, float blue) -> bool;
        auto get_color_mod(Uint8 *red, Uint8 *green, Uint8 *blue) const -> bool;
        auto get_color_mod(float *red, float *green, float *blue) const -> bool;

        // Alpha modulation
        auto set_alpha_mod(Uint8 alpha) -> bool;
        auto set_alpha_mod(float alpha) -> bool;
        auto get_alpha_mod(Uint8 *alpha) const -> bool;
        auto get_alpha_mod(float *alpha) const -> bool;

        // Blend mode
        auto set_blend_mode(SDL_BlendMode blend_mode) -> bool;
        auto get_blend_mode(SDL_BlendMode *blend_mode) const -> bool;

        // Scale mode
        auto set_scale_mode(SDL_ScaleMode scale_mode) -> bool;
        auto get_scale_mode(SDL_ScaleMode *scale_mode) const -> bool;

        // Update texture with pixel data
        auto update(const SDL_Rect *rect, const void *pixels, int pitch) -> bool;

        // Lock texture for direct pixel access
        auto lock(const SDL_Rect *rect, void **pixels, int *pitch) -> bool;
        void unlock();

        // Release the texture (call SDL_DestroyTexture)
        void release();

        // Render texture to renderer
        auto render(SDL_Renderer *renderer) const -> bool;
        auto render(SDL_Renderer *renderer, const Rectf &dst_rect) const -> bool;
        auto render(SDL_Renderer *renderer, const Rectf &src_rect, const Rectf &dst_rect) const
            -> bool;
        auto render(SDL_Renderer *renderer, float x_pos, float y_pos) const -> bool;
        auto render(SDL_Renderer *renderer, const Vector2f &position) const -> bool;

    private:
        SDL_Texture *m_texture;
    };
} // namespace Tactics
