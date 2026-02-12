#pragma once

#include "Tactics/Components/Camera.hpp"
#include "Tactics/Components/Cursor.hpp"

#include <SDL3/SDL.h>

namespace Tactics
{
    class CursorRenderer
    {
    public:
        CursorRenderer() = default;
        ~CursorRenderer() = default;

        CursorRenderer(const CursorRenderer &) = delete;
        auto operator=(const CursorRenderer &) -> CursorRenderer & = delete;

        CursorRenderer(CursorRenderer &&) = delete;
        auto operator=(CursorRenderer &&) -> CursorRenderer & = delete;

        [[nodiscard]] static auto render(SDL_Renderer *renderer, const Cursor &cursor,
                                         const Camera &camera, float tile_size) -> bool;
    };
} // namespace Tactics
