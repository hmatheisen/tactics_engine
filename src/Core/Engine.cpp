#include <Tactics/Core/Engine.hpp>
#include <Tactics/Core/Logger.hpp>

namespace Tactics
{
    namespace
    {
        constexpr int WINDOW_WIDTH = 1280;
        constexpr int WINDOW_HEIGHT = 720;
    } // namespace

    Engine::Engine() = default;

    Engine::~Engine()
    {
        shutdown();
    }

    auto Engine::initialize() -> bool
    {
        log_info("Initializing Engine...");

        // Initialize SDL3 video subsystem
        if (!SDL_Init(SDL_INIT_VIDEO))
        {
            log_error("Failed to initialize SDL video subsystem");
            return false;
        }
        log_debug("SDL video subsystem initialized");

        // Create window (1280x720)
        SDL_Window *window = SDL_CreateWindow("Tactics", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
        if (window == nullptr)
        {
            log_error("Failed to create window: " + std::string(SDL_GetError()));
            SDL_Quit();
            return false;
        }
        m_window = SDLWindowPtr(window);
        log_info("Window created: " + std::to_string(WINDOW_WIDTH) + "x" +
                 std::to_string(WINDOW_HEIGHT));

        // Create renderer
        SDL_Renderer *renderer = SDL_CreateRenderer(m_window.get(), nullptr);
        if (renderer == nullptr)
        {
            log_error("Failed to create renderer: " + std::string(SDL_GetError()));
            m_window.reset(); // Destroy window before quitting
            SDL_Quit();
            return false;
        }
        m_renderer = SDLRendererPtr(renderer);
        log_info("Renderer created");

        log_info("Engine initialized successfully");
        return true;
    }

    auto Engine::get_renderer() const -> SDL_Renderer *
    {
        return m_renderer.get();
    }

    void Engine::shutdown()
    {
        log_info("Shutting down Engine...");

        // Smart pointers will automatically destroy renderer and window
        // Renderer is destroyed first (declared after window), then window
        m_renderer.reset();
        log_debug("Renderer destroyed");
        m_window.reset();
        log_debug("Window destroyed");

        SDL_Quit();
        log_info("Engine shutdown complete");
    }

} // namespace Tactics
