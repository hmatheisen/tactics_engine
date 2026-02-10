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

        // Initialize SDL and create the main window/renderer.
        // Returns true on success.
        auto initialize() -> bool;

        void run();

        // Cleanup SDL resources. Safe to call multiple times.
        void shutdown();

        // Get the SDL renderer for rendering operations.
        [[nodiscard]] auto get_renderer() const -> SDL_Renderer *;

    private:
        SDLWindowPtr m_window;
        SDLRendererPtr m_renderer;

        bool m_is_running = false;
    };
} // namespace Tactics
