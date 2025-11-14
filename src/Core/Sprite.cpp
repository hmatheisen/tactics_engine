#include <Tactics/Core/Sprite.hpp>

namespace Tactics
{
    Sprite::Sprite()
        : m_position(0.0F, 0.0F), m_size(0.0F, 0.0F), m_source_rect(0.0F, 0.0F, 0.0F, 0.0F),
          m_use_source_rect(false)
    {}

    Sprite::Sprite(Texture texture)
        : m_texture(std::move(texture)), m_position(0.0F, 0.0F), m_size(0.0F, 0.0F),
          m_source_rect(0.0F, 0.0F, 0.0F, 0.0F), m_use_source_rect(false)
    {
        if (m_texture.is_valid())
        {
            m_size = m_texture.get_size();
        }
    }

    void Sprite::set_texture(Texture texture)
    {
        m_texture = std::move(texture);
        if (m_texture.is_valid() && (m_size.x == 0.0F || m_size.y == 0.0F))
        {
            m_size = m_texture.get_size();
        }
    }

    auto Sprite::get_texture() const -> const Texture &
    {
        return m_texture;
    }

    auto Sprite::is_valid() const -> bool
    {
        return m_texture.is_valid();
    }

    auto Sprite::get_position() const -> Vector2f
    {
        return m_position;
    }

    void Sprite::set_position(const Vector2f &position)
    {
        m_position = position;
    }

    auto Sprite::get_size() const -> Vector2f
    {
        return m_size;
    }

    void Sprite::set_size(const Vector2f &size)
    {
        m_size = size;
    }

    auto Sprite::get_source_rect() const -> Rectf
    {
        return m_source_rect;
    }

    void Sprite::set_source_rect(const Rectf &rect)
    {
        m_source_rect = rect;
        m_use_source_rect = true;
    }

    auto Sprite::render(SDL_Renderer *renderer) const -> bool
    {
        if (!m_texture.is_valid() || renderer == nullptr)
        {
            return false;
        }

        Rectf dst_rect(m_position.x, m_position.y, m_size.x, m_size.y);

        if (m_use_source_rect)
        {
            return m_texture.render(renderer, m_source_rect, dst_rect);
        }

        return m_texture.render(renderer, dst_rect);
    }
} // namespace Tactics
