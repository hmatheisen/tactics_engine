#pragma once

#include <SDL3/SDL.h>
#include <memory>

namespace Tactics
{
    // Custom deleters for SDL resources
    struct SDLWindowDeleter
    {
        void operator()(SDL_Window *window) const
        {
            if (window != nullptr)
            {
                SDL_DestroyWindow(window);
            }
        }
    };

    struct SDLRendererDeleter
    {
        void operator()(SDL_Renderer *renderer) const
        {
            if (renderer != nullptr)
            {
                SDL_DestroyRenderer(renderer);
            }
        }
    };

    using SDLWindowPtr = std::unique_ptr<SDL_Window, SDLWindowDeleter>;
    using SDLRendererPtr = std::unique_ptr<SDL_Renderer, SDLRendererDeleter>;

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

        SDLWindowPtr m_window;
        SDLRendererPtr m_renderer;
        bool m_running;
        Uint64 m_performance_frequency;
        Uint64 m_last_frame_time;
    };
} // namespace Tactics
