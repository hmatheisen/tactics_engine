#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <typeindex>
#include <unordered_map>

namespace Tactics
{
    class EventBus
    {
    public:
        EventBus() = default;
        ~EventBus() = default;

        EventBus(const EventBus &) = delete;
        auto operator=(const EventBus &) -> EventBus & = delete;

        EventBus(EventBus &&) = delete;
        auto operator=(EventBus &&) -> EventBus & = delete;

        [[nodiscard]] static auto instance() -> EventBus &;

        template <typename Event>
        using Handler = std::function<void(const Event &)>;

        template <typename Event>
        [[nodiscard]] auto subscribe(Handler<Event> handler) -> std::size_t;

        template <typename Event>
        void unsubscribe(std::size_t subscription_id);

        template <typename Event>
        void publish(const Event &event);

    private:
        struct IHandlerCollection
        {
            IHandlerCollection() = default;
            virtual ~IHandlerCollection() = default;

            IHandlerCollection(const IHandlerCollection &) = delete;
            auto operator=(const IHandlerCollection &) -> IHandlerCollection & = delete;

            IHandlerCollection(IHandlerCollection &&) = delete;
            auto operator=(IHandlerCollection &&) -> IHandlerCollection & = delete;
        };

        template <typename Event>
        struct HandlerCollection : IHandlerCollection
        {
            std::unordered_map<std::size_t, Handler<Event>> handlers;
        };

        template <typename Event>
        auto get_or_create_collection() -> HandlerCollection<Event> &;

        std::unordered_map<std::type_index, std::unique_ptr<IHandlerCollection>> m_collections;
        std::size_t m_next_subscription_id{1U};
    };

    class Subscriber
    {
    public:
        template <typename Event, typename Handler>
        [[nodiscard]] static auto subscribe(Handler handler) -> std::size_t
        {
            return EventBus::instance().subscribe<Event>(std::move(handler));
        }

        template <typename Event>
        static void unsubscribe(std::size_t subscription_id)
        {
            EventBus::instance().unsubscribe<Event>(subscription_id);
        }
    };

    class Publisher
    {
    public:
        template <typename Event>
        static void publish(const Event &event)
        {
            EventBus::instance().publish(event);
        }
    };

} // namespace Tactics

// Template implementation
namespace Tactics
{
    template <typename Event>
    auto EventBus::get_or_create_collection() -> HandlerCollection<Event> &
    {
        const std::type_index type_key(typeid(Event));
        auto it_collection = m_collections.find(type_key);
        if (it_collection == m_collections.end())
        {
            auto collection = std::make_unique<HandlerCollection<Event>>();
            auto *collection_raw = collection.get();
            m_collections.emplace(type_key, std::move(collection));
            return *collection_raw;
        }

        return *static_cast<HandlerCollection<Event> *>(it_collection->second.get());
    }

    template <typename Event>
    auto EventBus::subscribe(Handler<Event> handler) -> std::size_t
    {
        auto &collection = get_or_create_collection<Event>();
        const std::size_t subscription_id = m_next_subscription_id++;
        collection.handlers.emplace(subscription_id, std::move(handler));
        return subscription_id;
    }

    template <typename Event>
    void EventBus::unsubscribe(std::size_t subscription_id)
    {
        if (subscription_id == 0U)
        {
            return;
        }

        const std::type_index type_key(typeid(Event));
        const auto it_collection = m_collections.find(type_key);
        if (it_collection == m_collections.end())
        {
            return;
        }

        auto *collection = static_cast<HandlerCollection<Event> *>(it_collection->second.get());
        collection->handlers.erase(subscription_id);
    }

    template <typename Event>
    void EventBus::publish(const Event &event)
    {
        const std::type_index type_key(typeid(Event));
        const auto it_collection = m_collections.find(type_key);
        if (it_collection == m_collections.end())
        {
            return;
        }

        const auto *collection =
            static_cast<const HandlerCollection<Event> *>(it_collection->second.get());
        for (const auto &entry : collection->handlers)
        {
            const auto &handler = entry.second;
            if (handler)
            {
                handler(event);
            }
        }
    }
} // namespace Tactics
