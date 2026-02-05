#pragma once

#include <SDL3/SDL.h>

namespace Tactics
{
    class TimeManager
    {
    public:
        TimeManager() = default;
        ~TimeManager() = default;

        // Delete copy constructor and assignment operator
        TimeManager(const TimeManager &) = delete;
        auto operator=(const TimeManager &) -> TimeManager & = delete;

        // Delete move constructor and assignment operator
        TimeManager(TimeManager &&) = delete;
        auto operator=(TimeManager &&) -> TimeManager & = delete;

        // Initialize the time manager (call once at startup)
        void initialize();

        // Update timing and calculate delta time (call at start of each frame)
        void update();

        // Get delta time in seconds
        [[nodiscard]] auto get_delta_time() const -> float;

        // Get current frame time in seconds
        [[nodiscard]] auto get_frame_time() const -> float;

        // Frame rate limiting
        void set_target_fps(float target_fps);
        [[nodiscard]] auto get_target_fps() const -> float;

        // Cap frame rate (call at end of frame)
        void cap_frame_rate();

    private:
        static constexpr float DEFAULT_TARGET_FPS = 60.0F;
        static constexpr float MILLISECONDS_PER_SECOND = 1000.0F;

        Uint64 m_performance_frequency{0};
        Uint64 m_last_frame_time{0};
        Uint64 m_current_frame_start_time{0};
        float m_delta_time{0.0F};
        float m_frame_time{0.0F};
        float m_target_fps{DEFAULT_TARGET_FPS};
        float m_target_frame_time{1.0F / DEFAULT_TARGET_FPS};
    };
} // namespace Tactics
