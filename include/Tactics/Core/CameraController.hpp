#pragma once

#include <Tactics/Core/Camera.hpp>
#include <Tactics/Core/Cursor.hpp>

namespace Tactics
{
    class CameraController
    {
    public:
        CameraController();
        ~CameraController() = default;

        // Delete copy constructor and assignment operator
        CameraController(const CameraController &) = delete;
        auto operator=(const CameraController &) -> CameraController & = delete;

        // Delete move constructor and assignment operator
        CameraController(CameraController &&) = delete;
        auto operator=(CameraController &&) -> CameraController & = delete;

        // Update camera based on cursor position (edge scrolling)
        void update(Camera &camera, const Cursor &cursor, float tile_size) const;

        // Edge scroll threshold accessors
        void set_edge_scroll_threshold(float threshold);
        [[nodiscard]] auto get_edge_scroll_threshold() const -> float;

    private:
        float m_edge_scroll_threshold;
    };
} // namespace Tactics
