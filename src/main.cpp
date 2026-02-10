#include "Tactics/Core/Engine.hpp"
#include "Tactics/Core/Logger.hpp"
#include "Tactics/Core/SQLiteGridRepository.hpp"
#include "Tactics/Core/SQLiteUnitRepository.hpp"
#include "Tactics/Core/SceneManager.hpp"
#include "Tactics/Scenes/GridScene.hpp"
#include <SDL3/SDL.h>
#include <cstdlib>
#include <memory>
#include <string_view>

auto main() -> int
{
    // Logger
    auto &logger = Tactics::Logger::instance();
    logger.set_level(Tactics::LogLevel::Debug);
    logger.set_file_logging(true, "tactics.log");

    Tactics::log_info("=== Tactics Engine Starting ===");

    // Database
    Tactics::SQLiteGridRepository repository("maps.db");
    Tactics::SQLiteUnitRepository unit_repository("maps.db");
    const std::string_view default_map_name = "default";

    Tactics::Engine engine;

    if (!engine.initialize())
    {
        Tactics::log_error("Failed to initialize engine");
        return EXIT_FAILURE;
    }

    // Scenes
    auto &scene_manager = Tactics::SceneManager::instance();
    auto grid_scene = std::make_unique<Tactics::GridScene>(&repository, &unit_repository,
                                                           default_map_name.data());
    scene_manager.change_scene(std::move(grid_scene));

    // Blocking main game loop
    engine.run();

    engine.shutdown();
    Tactics::log_info("=== Tactics Engine Shutting Down ===");

    return EXIT_SUCCESS;
}
