#include <Tactics/Core/Engine.hpp>
#include <Tactics/Core/Logger.hpp>
#include <cstdlib>

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

    engine.run();
    engine.shutdown();

    Tactics::log_info("=== Tactics Engine Shutting Down ===");
    return EXIT_SUCCESS;
}
