#include <Tactics/Core/Engine.hpp>
#include <cstdlib>

auto main() -> int
{
    Tactics::Engine engine;

    if (!engine.initialize())
    {
        return EXIT_FAILURE;
    }

    engine.run();
    engine.shutdown();

    return EXIT_SUCCESS;
}
