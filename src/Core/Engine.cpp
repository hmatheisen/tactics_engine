#include <Tactics/Core/Engine.hpp>

namespace Tactics
{

    namespace
    {
        constexpr int WINDOW_WIDTH = 1280;
        constexpr int WINDOW_HEIGHT = 720;

        constexpr Uint8 CLEAR_COLOR_R = 0x2E;
        constexpr Uint8 CLEAR_COLOR_G = 0x2E;
        constexpr Uint8 CLEAR_COLOR_B = 0x2E;
        constexpr Uint8 CLEAR_COLOR_A = 0xFF;
    } // namespace

    Engine::Engine() : m_running(false), m_performance_frequency(0), m_last_frame_time(0) {}

    Engine::~Engine()
    {
        shutdown();
    }

    auto Engine::initialize() -> bool
    {
        // Initialize SDL3 video subsystem
        if (!SDL_Init(SDL_INIT_VIDEO))
        {
            return false;
        }

        // Get performance frequency for delta time calculation
        m_performance_frequency = SDL_GetPerformanceFrequency();

        // Create window (1280x720)
        SDL_Window *window = SDL_CreateWindow("Tactics", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
        if (window == nullptr)
        {
            SDL_Quit();
            return false;
        }
        m_window = SDLWindowPtr(window);

        // Create renderer
        SDL_Renderer *renderer = SDL_CreateRenderer(m_window.get(), nullptr);
        if (renderer == nullptr)
        {
            m_window.reset(); // Destroy window before quitting
            SDL_Quit();
            return false;
        }
        m_renderer = SDLRendererPtr(renderer);

        // Initialize timing
        m_last_frame_time = SDL_GetPerformanceCounter();
        m_running = true;

        return true;
    }

    void Engine::run()
    {
        if (!m_running)
        {
            return;
        }

        while (m_running)
        {
            // Calculate delta time
            Uint64 current_time = SDL_GetPerformanceCounter();
            float delta_time = static_cast<float>(current_time - m_last_frame_time) /
                               static_cast<float>(m_performance_frequency);
            m_last_frame_time = current_time;

            // Process events
            process_events();

            // Update game logic
            update(delta_time);

            // Render frame
            render();
        }
    }

    void Engine::shutdown()
    {
        // Smart pointers will automatically destroy renderer and window
        // Renderer is destroyed first (declared after window), then window
        m_renderer.reset();
        m_window.reset();

        SDL_Quit();
        m_running = false;
    }

    void Engine::process_events()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                m_running = false;
            }
        }
    }

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    void Engine::update(float delta_time)
    {
        // Game logic update will go here
        (void)delta_time; // Suppress unused parameter warning for now
    }

    void Engine::render()
    {
        // Clear screen to a color (dark gray)
        SDL_SetRenderDrawColor(m_renderer.get(), CLEAR_COLOR_R, CLEAR_COLOR_G, CLEAR_COLOR_B,
                               CLEAR_COLOR_A);
        SDL_RenderClear(m_renderer.get());

        // Present the rendered frame
        SDL_RenderPresent(m_renderer.get());
    }

} // namespace Tactics
