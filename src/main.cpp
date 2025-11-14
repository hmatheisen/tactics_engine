#include <SDL3/SDL.h>
#include <Tactics/Core/Engine.hpp>
#include <Tactics/Core/Logger.hpp>
#include <Tactics/Core/SceneManager.hpp>
#include <Tactics/Scenes/GridScene.hpp>
#include <cstdlib>
#include <memory>

auto main() -> int
{
    // Initialize logger
    Tactics::Logger::instance().set_level(Tactics::LogLevel::Info);
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

    // Main game loop
    Uint64 last_frame_time = SDL_GetPerformanceCounter();
    const Uint64 performance_frequency = SDL_GetPerformanceFrequency();

    while (scene_manager.is_running())
    {
        // Calculate delta time
        Uint64 current_time = SDL_GetPerformanceCounter();
        float delta_time = static_cast<float>(current_time - last_frame_time) /
                           static_cast<float>(performance_frequency);
        last_frame_time = current_time;

        // Update and render current scene
        scene_manager.update(delta_time);
        scene_manager.render(engine.get_renderer());
        SDL_RenderPresent(engine.get_renderer());
    }

    engine.shutdown();

    Tactics::log_info("=== Tactics Engine Shutting Down ===");
    return EXIT_SUCCESS;
}
