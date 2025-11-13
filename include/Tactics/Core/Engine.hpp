#pragma once

#include <SDL3/SDL.h>

namespace Tactics
{

    class Engine
    {
    public:
        Engine();
        ~Engine();

        // Delete copy constructor and assignment operator
        Engine(const Engine &) = delete;
        auto operator=(const Engine &) -> Engine & = delete;

        // Delete move constructor and assignment operator
        Engine(Engine &&) = delete;
        auto operator=(Engine &&) -> Engine & = delete;

        // Initialize the engine
        auto initialize() -> bool;

        // Run the main game loop
        void run();

        // Cleanup resources
        void shutdown();

    private:
        // Handle SDL events
        void process_events();

        // Update game logic
        void update(float delta_time);

        // Render the frame
        void render();

        SDL_Window *m_window;
        SDL_Renderer *m_renderer;
        bool m_running;
        Uint64 m_performance_frequency;
        Uint64 m_last_frame_time;
    };

} // namespace Tactics
