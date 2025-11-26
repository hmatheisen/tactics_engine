#include <Tactics/Core/EventBus.hpp>

namespace Tactics
{
    auto EventBus::instance() -> EventBus &
    {
        static EventBus s_instance;
        return s_instance;
    }
} // namespace Tactics
