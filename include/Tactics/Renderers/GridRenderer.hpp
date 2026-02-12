#pragma once

#include "Tactics/Components/Camera.hpp"
#include "Tactics/Components/Grid.hpp"

#include <SDL3/SDL.h>

namespace Tactics
{
    class GridRenderer
    {
    public:
        GridRenderer() = default;
        ~GridRenderer() = default;

        GridRenderer(const GridRenderer &) = delete;
        auto operator=(const GridRenderer &) -> GridRenderer & = delete;

        GridRenderer(GridRenderer &&) = delete;
        auto operator=(GridRenderer &&) -> GridRenderer & = delete;

        [[nodiscard]] static auto render(SDL_Renderer *renderer, const Grid &grid,
                                         const Camera &camera, float tile_size) -> bool;
    };
} // namespace Tactics
