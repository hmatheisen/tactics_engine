#include <Tactics/Core/CursorEvents.hpp>
#include <Tactics/Core/EventBus.hpp>
#include <utility>

namespace Tactics
{
    auto CursorEvents::subscribe_moved(CursorMovedHandler handler) -> std::size_t
    {
        return EventBus::instance().subscribe<CursorMovedEvent>(std::move(handler));
    }

    void CursorEvents::unsubscribe_moved(std::size_t subscription_id)
    {
        EventBus::instance().unsubscribe<CursorMovedEvent>(subscription_id);
    }

    void CursorEvents::publish_moved(const Vector2i &position)
    {
        EventBus::instance().publish(CursorMovedEvent{position});
    }
} // namespace Tactics
