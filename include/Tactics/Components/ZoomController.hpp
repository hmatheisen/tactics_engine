#pragma once

#include "Tactics/Components/Camera.hpp"

namespace Tactics
{
    class ZoomController
    {
    public:
        ZoomController() = default;
        ~ZoomController() = default;

        // Delete copy constructor and assignment operator
        ZoomController(const ZoomController &) = delete;
        auto operator=(const ZoomController &) -> ZoomController & = delete;

        // Delete move constructor and assignment operator
        ZoomController(ZoomController &&) = delete;
        auto operator=(ZoomController &&) -> ZoomController & = delete;

        void update(Camera &camera, float delta_time);

    private:
        static constexpr float KEY_REPEAT_INITIAL_DELAY = 0.3F; // Seconds before first repeat
        static constexpr float KEY_REPEAT_RATE = 0.06F;         // Seconds between repeats

        static constexpr float ZOOM_OUT_FACTOR = 0.9F;
        static constexpr float ZOOM_IN_FACTOR = 1.1F;

        float m_zoom_repeat_timer = 0.0F;

        static void zoom_in(Camera &camera);
        static void zoom_out(Camera &camera);
    };
} // namespace Tactics
