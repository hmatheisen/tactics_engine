#pragma once

#include <Tactics/Core/Camera.hpp>
#include <Tactics/Core/Vector2.hpp>
#include <cstddef>

namespace Tactics
{
    class CameraController
    {
    public:
        CameraController();
        ~CameraController();

        // Delete copy constructor and assignment operator
        CameraController(const CameraController &) = delete;
        auto operator=(const CameraController &) -> CameraController & = delete;

        // Delete move constructor and assignment operator
        CameraController(CameraController &&) = delete;
        auto operator=(CameraController &&) -> CameraController & = delete;

        // Update camera based on cursor position (edge scrolling)
        void update(Camera &camera, float tile_size) const;

        // Edge scroll threshold accessors
        void set_edge_scroll_threshold(float threshold);
        [[nodiscard]] auto get_edge_scroll_threshold() const -> float;

    private:
        float m_edge_scroll_threshold;
        Vector2i m_cursor_tile_position;
        std::size_t m_cursor_subscription_id{0U};
    };
} // namespace Tactics
