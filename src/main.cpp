#include <SDL3/SDL.h>
#include <Tactics/Core/Engine.hpp>
#include <Tactics/Core/InputManager.hpp>
#include <Tactics/Core/Logger.hpp>
#include <Tactics/Core/SceneManager.hpp>
#include <Tactics/Core/TimeManager.hpp>
#include <Tactics/Scenes/GridScene.hpp>
#include <cstdlib>
#include <memory>

auto main() -> int
{
    // Initialize logger
    Tactics::Logger::instance().set_level(Tactics::LogLevel::Debug);
    Tactics::Logger::instance().set_file_logging(true, "tactics.log");

    Tactics::log_info("=== Tactics Engine Starting ===");

    Tactics::Engine engine;

    if (!engine.initialize())
    {
        Tactics::log_error("Failed to initialize engine");
        return EXIT_FAILURE;
    }

    // Create scene manager and initial scene
    Tactics::SceneManager scene_manager;
    scene_manager.change_scene(std::make_unique<Tactics::GridScene>());

    // Create time manager
    Tactics::TimeManager time_manager;
    time_manager.initialize();
    constexpr float TARGET_FPS = 60.0F;
    time_manager.set_target_fps(TARGET_FPS);

    bool running = true;

    // Main game loop
    while (running && scene_manager.is_running())
    {
        // Process SDL events once per frame and feed them to the input system
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            // Global quit handling
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }

            // Dispatch event to input manager (mouse wheel, etc.)
            Tactics::InputManager::instance().process_event(event);
        }

        // Update input snapshot for this frame
        Tactics::InputManager::instance().update();

        // Update timing and get delta time
        time_manager.update();

        // Update and render current scene
        scene_manager.update(time_manager.get_delta_time());
        scene_manager.render(engine.get_renderer());
        SDL_RenderPresent(engine.get_renderer());

        // Cap frame rate
        time_manager.cap_frame_rate();
    }

    engine.shutdown();

    Tactics::log_info("=== Tactics Engine Shutting Down ===");
    return EXIT_SUCCESS;
}
