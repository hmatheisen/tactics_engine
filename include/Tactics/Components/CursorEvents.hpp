#pragma once

#include "Tactics/Core/Vector2.hpp"
#include <cstddef>
#include <functional>

namespace Tactics
{
    namespace CursorEvents
    {
        struct Moved
        {
            Vector2i position;
        };
        using MovedHandler = std::function<void(const Moved &)>;
    }; // namespace CursorEvents
} // namespace Tactics
