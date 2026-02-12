#pragma once

#include "Tactics/Core/Rect.hpp"
#include "Tactics/Core/Vector2.hpp"
#include <SDL3/SDL.h>
#include <string>

namespace Tactics
{
    class Texture
    {
    public:
        Texture() = default;
        ~Texture();

        // Delete copy constructor and assignment operator
        Texture(const Texture &) = delete;
        auto operator=(const Texture &) -> Texture & = delete;

        // Move constructor
        Texture(Texture &&other) noexcept;

        // Move assignment operator
        auto operator=(Texture &&other) noexcept -> Texture &;

        // Create texture from renderer
        [[nodiscard]] static auto create(SDL_Renderer *renderer, SDL_PixelFormat format,
                                         SDL_TextureAccess access, int width, int height)
            -> Texture;

        // Create texture from surface
        [[nodiscard]] static auto create_from_surface(SDL_Renderer *renderer, SDL_Surface *surface)
            -> Texture;

        // Load texture from file (BMP format)
        [[nodiscard]] static auto load_from_file(SDL_Renderer *renderer,
                                                 const std::string &file_path) -> Texture;

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
        [[nodiscard]] auto set_color_mod(Uint8 red, Uint8 green, Uint8 blue) -> bool;
        [[nodiscard]] auto set_color_mod(float red, float green, float blue) -> bool;
        [[nodiscard]] auto get_color_mod(Uint8 *red, Uint8 *green, Uint8 *blue) const -> bool;
        [[nodiscard]] auto get_color_mod(float *red, float *green, float *blue) const -> bool;

        // Alpha modulation
        [[nodiscard]] auto set_alpha_mod(Uint8 alpha) -> bool;
        [[nodiscard]] auto set_alpha_mod(float alpha) -> bool;
        [[nodiscard]] auto get_alpha_mod(Uint8 *alpha) const -> bool;
        [[nodiscard]] auto get_alpha_mod(float *alpha) const -> bool;

        // Blend mode
        [[nodiscard]] auto set_blend_mode(SDL_BlendMode blend_mode) -> bool;
        [[nodiscard]] auto get_blend_mode(SDL_BlendMode *blend_mode) const -> bool;

        // Scale mode
        [[nodiscard]] auto set_scale_mode(SDL_ScaleMode scale_mode) -> bool;
        [[nodiscard]] auto get_scale_mode(SDL_ScaleMode *scale_mode) const -> bool;

        // Update texture with pixel data
        [[nodiscard]] auto update(const SDL_Rect *rect, const void *pixels, int pitch) -> bool;

        // Lock texture for direct pixel access
        [[nodiscard]] auto lock(const SDL_Rect *rect, void **pixels, int *pitch) -> bool;
        void unlock();

        // Release the texture (call SDL_DestroyTexture)
        void release();

        // Render texture to renderer
        [[nodiscard]] auto render(SDL_Renderer *renderer) const -> bool;
        [[nodiscard]] auto render(SDL_Renderer *renderer, const Rectf &dst_rect) const -> bool;
        [[nodiscard]] auto render(SDL_Renderer *renderer, const Rectf &src_rect,
                                  const Rectf &dst_rect) const -> bool;
        [[nodiscard]] auto render(SDL_Renderer *renderer, float x_pos, float y_pos) const -> bool;
        [[nodiscard]] auto render(SDL_Renderer *renderer, const Vector2f &position) const -> bool;

    private:
        SDL_Texture *m_texture{nullptr};
    };
} // namespace Tactics
