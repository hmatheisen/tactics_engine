#include "Tactics/Core/TimeManager.hpp"

namespace Tactics
{
    void TimeManager::initialize()
    {
        m_performance_frequency = SDL_GetPerformanceFrequency();
        m_last_frame_time = SDL_GetPerformanceCounter();
        m_current_frame_start_time = m_last_frame_time;
    }

    void TimeManager::update()
    {
        // Record frame start time
        m_current_frame_start_time = SDL_GetPerformanceCounter();

        // Calculate delta time
        m_delta_time = static_cast<float>(m_current_frame_start_time - m_last_frame_time) /
                       static_cast<float>(m_performance_frequency);

        // Update last frame time for next frame
        m_last_frame_time = m_current_frame_start_time;
    }

    auto TimeManager::get_delta_time() const -> float
    {
        return m_delta_time;
    }

    auto TimeManager::get_frame_time() const -> float
    {
        return m_frame_time;
    }

    void TimeManager::set_target_fps(float target_fps)
    {
        if (target_fps > 0.0F)
        {
            m_target_fps = target_fps;
            m_target_frame_time = 1.0F / target_fps;
        }
    }

    auto TimeManager::get_target_fps() const -> float
    {
        return m_target_fps;
    }

    void TimeManager::cap_frame_rate()
    {
        // Calculate how long the frame took
        const Uint64 frame_end_time = SDL_GetPerformanceCounter();
        m_frame_time = static_cast<float>(frame_end_time - m_current_frame_start_time) /
                       static_cast<float>(m_performance_frequency);

        // Sleep if frame was faster than target
        const float sleep_time = m_target_frame_time - m_frame_time;
        if (sleep_time > 0.0F)
        {
            SDL_Delay(static_cast<Uint32>(sleep_time * MILLISECONDS_PER_SECOND));
        }
    }
} // namespace Tactics
