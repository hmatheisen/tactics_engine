#include "Tactics/Components/CameraController.hpp"
#include "Tactics/Components/CursorEvents.hpp"

namespace Tactics
{
    namespace
    {} // namespace

    CameraController::CameraController()
        : m_cursor_subscription_id(subscribe<CursorEvents::Moved, CursorEvents::MovedHandler>(
              [this](const CursorEvents::Moved &event) -> void
              { m_cursor_world_position = event.world_position; }))
    {}

    CameraController::~CameraController()
    {
        unsubscribe<CursorEvents::Moved>(m_cursor_subscription_id);
    }

    void CameraController::update(Camera &camera) const
    {
        // Convert cursor world position to screen position
        const Vector2f cursor_screen_pos = camera.world_to_screen(m_cursor_world_position);

        // Calculate camera movement based on cursor proximity to edges
        Vector2f camera_movement(0.0F, 0.0F);
        const float viewport_width = camera.get_viewport_width();
        const float viewport_height = camera.get_viewport_height();

        // Check left edge - only scroll if cursor is too close to edge
        if (cursor_screen_pos.x < m_edge_scroll_threshold)
        {
            // Calculate how much we need to move to keep cursor at threshold distance
            const float distance_to_move = m_edge_scroll_threshold - cursor_screen_pos.x;
            // Convert screen distance to world distance
            const float world_distance = distance_to_move / camera.get_zoom();
            camera_movement.x -= world_distance;
        }

        // Check right edge
        if (cursor_screen_pos.x > viewport_width - m_edge_scroll_threshold)
        {
            // Calculate how much we need to move to keep cursor at threshold distance
            const float distance_to_move =
                cursor_screen_pos.x - (viewport_width - m_edge_scroll_threshold);
            // Convert screen distance to world distance
            const float world_distance = distance_to_move / camera.get_zoom();
            camera_movement.x += world_distance;
        }

        // Check top edge
        if (cursor_screen_pos.y < m_edge_scroll_threshold)
        {
            // Calculate how much we need to move to keep cursor at threshold distance
            const float distance_to_move = m_edge_scroll_threshold - cursor_screen_pos.y;
            // Convert screen distance to world distance
            const float world_distance = distance_to_move / camera.get_zoom();
            camera_movement.y -= world_distance;
        }

        // Check bottom edge
        if (cursor_screen_pos.y > viewport_height - m_edge_scroll_threshold)
        {
            // Calculate how much we need to move to keep cursor at threshold distance
            const float distance_to_move =
                cursor_screen_pos.y - (viewport_height - m_edge_scroll_threshold);
            // Convert screen distance to world distance
            const float world_distance = distance_to_move / camera.get_zoom();
            camera_movement.y += world_distance;
        }

        // Apply camera movement (moves just enough to keep cursor within threshold)
        if (camera_movement.x != 0.0F || camera_movement.y != 0.0F)
        {
            camera.translate(camera_movement);
        }
    }

    void CameraController::set_edge_scroll_threshold(float threshold)
    {
        m_edge_scroll_threshold = threshold;
    }

    auto CameraController::get_edge_scroll_threshold() const -> float
    {
        return m_edge_scroll_threshold;
    }
} // namespace Tactics
