#pragma once

#include "Tactics/Core/Rect.hpp"
#include "Tactics/Core/Texture.hpp"
#include "Tactics/Core/Vector2.hpp"

namespace Tactics
{
    class Sprite
    {
    public:
        Sprite();
        explicit Sprite(Texture texture);
        ~Sprite() = default;

        // Delete copy constructor and assignment operator
        Sprite(const Sprite &) = delete;
        auto operator=(const Sprite &) -> Sprite & = delete;

        // Move constructor
        Sprite(Sprite &&other) noexcept = default;

        // Move assignment operator
        auto operator=(Sprite &&other) noexcept -> Sprite & = default;

        // Set the texture
        void set_texture(Texture texture);

        // Get the texture
        [[nodiscard]] auto get_texture() const -> const Texture &;

        // Check if sprite has a valid texture
        [[nodiscard]] auto is_valid() const -> bool;

        // Position accessors
        [[nodiscard]] auto get_position() const -> Vector2f;
        void set_position(const Vector2f &position);

        // Size accessors (defaults to texture size)
        [[nodiscard]] auto get_size() const -> Vector2f;
        void set_size(const Vector2f &size);

        // Source rectangle (for sprite sheets)
        [[nodiscard]] auto get_source_rect() const -> Rectf;
        void set_source_rect(const Rectf &rect);

        // Render the sprite
        [[nodiscard]] auto render(SDL_Renderer *renderer) const -> bool;

    private:
        Texture m_texture;
        Vector2f m_position{};
        Vector2f m_size{};
        Rectf m_source_rect{};
        bool m_use_source_rect{false};
    };
} // namespace Tactics
