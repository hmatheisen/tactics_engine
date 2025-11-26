#pragma once

#include <Tactics/Core/Vector2.hpp>
#include <cstddef>
#include <functional>

namespace Tactics
{
    struct CursorMovedEvent
    {
        Vector2i position;
    };

    class CursorEvents
    {
    public:
        using CursorMovedHandler = std::function<void(const CursorMovedEvent &)>;

        [[nodiscard]] static auto subscribe_moved(CursorMovedHandler handler) -> std::size_t;
        static void unsubscribe_moved(std::size_t subscription_id);
        static void publish_moved(const Vector2i &position);
    };
} // namespace Tactics
