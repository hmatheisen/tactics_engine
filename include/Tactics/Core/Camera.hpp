#pragma once

#include "Tactics/Core/Rect.hpp"
#include "Tactics/Core/Vector2.hpp"

namespace Tactics
{
    struct CameraSettings
    {
        Vector2f position;
        float zoom;
        float viewport_width;
        float viewport_height;
    };

    class Camera
    {
    public:
        Camera() = default;
        explicit Camera(const CameraSettings &settings);
        ~Camera() = default;

        // Delete copy constructor and assignment operator
        Camera(const Camera &) = delete;
        auto operator=(const Camera &) -> Camera & = delete;

        // Move constructor
        Camera(Camera &&other) noexcept = default;

        // Move assignment operator
        auto operator=(Camera &&other) noexcept -> Camera & = default;

        // Position accessors
        [[nodiscard]] auto get_position() const -> Vector2f;
        void set_position(const Vector2f &position);

        // Zoom accessors
        [[nodiscard]] auto get_zoom() const -> float;
        void set_zoom(float zoom);

        // Viewport size accessors
        [[nodiscard]] auto get_viewport_width() const -> float;
        [[nodiscard]] auto get_viewport_height() const -> float;
        [[nodiscard]] auto get_viewport_size() const -> Vector2f;
        void set_viewport_size(const Vector2f &size);

        // Get the view rectangle in world coordinates
        [[nodiscard]] auto get_view_rect() const -> Rectf;

        // Convert world coordinates to screen coordinates
        [[nodiscard]] auto world_to_screen(const Vector2f &world_pos) const -> Vector2f;

        // Convert screen coordinates to world coordinates
        [[nodiscard]] auto screen_to_world(const Vector2f &screen_pos) const -> Vector2f;

        // Convert world rectangle to screen rectangle
        [[nodiscard]] auto world_to_screen_rect(const Rectf &world_rect) const -> Rectf;

        // Convert screen rectangle to world rectangle
        [[nodiscard]] auto screen_to_world_rect(const Rectf &screen_rect) const -> Rectf;

        // Move camera by offset
        void translate(const Vector2f &offset);

        // Set camera to center on a world position
        void center_on(const Vector2f &world_pos);

    private:
        static constexpr float DEFAULT_ZOOM = 1.0F;
        static constexpr float DEFAULT_VIEWPORT_WIDTH = 1280.0F;
        static constexpr float DEFAULT_VIEWPORT_HEIGHT = 720.0F;

        Vector2f m_position{0.0F, 0.0F};
        float m_zoom{DEFAULT_ZOOM};
        float m_viewport_width{DEFAULT_VIEWPORT_WIDTH};
        float m_viewport_height{DEFAULT_VIEWPORT_HEIGHT};
    };
} // namespace Tactics
