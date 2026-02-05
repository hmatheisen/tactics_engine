#include "Tactics/Core/Camera.hpp"
#include "Tactics/Core/Logger.hpp"
#include <algorithm>

namespace Tactics
{
    Camera::Camera(const CameraSettings &settings)
        : m_position(settings.position), m_zoom(settings.zoom),
          m_viewport_width(settings.viewport_width), m_viewport_height(settings.viewport_height)
    {
        if (m_zoom <= 0.0F)
        {
            log_warning("Camera zoom must be positive, setting to 1.0");
            m_zoom = 1.0F;
        }
        if (m_viewport_width <= 0.0F || m_viewport_height <= 0.0F)
        {
            log_warning("Camera viewport dimensions must be positive");
            m_viewport_width = std::max(m_viewport_width, 1.0F);
            m_viewport_height = std::max(m_viewport_height, 1.0F);
        }
    }

    auto Camera::get_position() const -> Vector2f
    {
        return m_position;
    }

    void Camera::set_position(const Vector2f &position)
    {
        m_position = position;
    }

    auto Camera::get_zoom() const -> float
    {
        return m_zoom;
    }

    void Camera::set_zoom(float zoom)
    {
        if (zoom <= 0.0F)
        {
            log_warning("Camera zoom must be positive, ignoring value: " + std::to_string(zoom));
            return;
        }
        m_zoom = zoom;
    }

    auto Camera::get_viewport_width() const -> float
    {
        return m_viewport_width;
    }

    auto Camera::get_viewport_height() const -> float
    {
        return m_viewport_height;
    }

    auto Camera::get_viewport_size() const -> Vector2f
    {
        return {m_viewport_width, m_viewport_height};
    }

    void Camera::set_viewport_size(const Vector2f &size)
    {
        const float width = size.x;
        const float height = size.y;

        if (width <= 0.0F || height <= 0.0F)
        {
            log_warning("Camera viewport dimensions must be positive");
            m_viewport_width = std::max(width, 1.0F);
            m_viewport_height = std::max(height, 1.0F);
        }

        m_viewport_width = width;
        m_viewport_height = height;
    }

    auto Camera::get_view_rect() const -> Rectf
    {
        // Calculate the world-space view rectangle
        // The viewport size divided by zoom gives the world-space size
        const float world_width = m_viewport_width / m_zoom;
        const float world_height = m_viewport_height / m_zoom;

        // Camera position is at the center of the view
        const float half_width = world_width * 0.5F;
        const float half_height = world_height * 0.5F;

        return {m_position.x - half_width, m_position.y - half_height, world_width, world_height};
    }

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    auto Camera::world_to_screen(const Vector2f &world_pos) const -> Vector2f
    {
        // Convert world coordinates to screen coordinates
        // Screen position = (world_pos - camera_pos) * zoom + viewport_center
        float screen_x = 0.0F;
        float screen_y = 0.0F;

        screen_x = ((world_pos.x - m_position.x) * m_zoom) + (m_viewport_width * 0.5F);
        screen_y = ((world_pos.y - m_position.y) * m_zoom) + (m_viewport_height * 0.5F);

        return {screen_x, screen_y};
    }

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    auto Camera::screen_to_world(const Vector2f &screen_pos) const -> Vector2f
    {
        // Convert screen coordinates to world coordinates
        // World position = (screen_pos - viewport_center) / zoom + camera_pos
        float world_x = 0.0F;
        float world_y = 0.0F;

        world_x = ((screen_pos.x - (m_viewport_width * 0.5F)) / m_zoom) + m_position.x;
        world_y = ((screen_pos.y - (m_viewport_height * 0.5F)) / m_zoom) + m_position.y;

        return {world_x, world_y};
    }

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    auto Camera::world_to_screen_rect(const Rectf &world_rect) const -> Rectf
    {
        // Convert world rectangle to screen rectangle
        const Vector2f top_left = world_to_screen(world_rect.top_left());
        const Vector2f bottom_right = world_to_screen(world_rect.bottom_right());

        const float screen_width = (bottom_right.x - top_left.x);
        const float screen_height = (bottom_right.y - top_left.y);

        return {top_left.x, top_left.y, screen_width, screen_height};
    }

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    auto Camera::screen_to_world_rect(const Rectf &screen_rect) const -> Rectf
    {
        // Convert screen rectangle to world rectangle
        const Vector2f top_left = screen_to_world(screen_rect.top_left());
        const Vector2f bottom_right = screen_to_world(screen_rect.bottom_right());

        const float world_width = bottom_right.x - top_left.x;
        const float world_height = bottom_right.y - top_left.y;

        return {top_left.x, top_left.y, world_width, world_height};
    }

    void Camera::translate(const Vector2f &offset)
    {
        m_position += offset;
    }

    void Camera::center_on(const Vector2f &world_pos)
    {
        m_position = world_pos;
    }

} // namespace Tactics
