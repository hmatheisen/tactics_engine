#pragma once

#include "Tactics/Components/Camera.hpp"
#include "Tactics/Components/Unit.hpp"

#include <SDL3/SDL.h>
#include <vector>

namespace Tactics
{
    class UnitRenderer
    {
    public:
        UnitRenderer() = default;
        ~UnitRenderer() = default;

        UnitRenderer(const UnitRenderer &) = delete;
        auto operator=(const UnitRenderer &) -> UnitRenderer & = delete;

        UnitRenderer(UnitRenderer &&) = delete;
        auto operator=(UnitRenderer &&) -> UnitRenderer & = delete;

        static void render_units(SDL_Renderer *renderer, const Camera &camera, float tile_size,
                                 const std::vector<Unit> &units);
    };
} // namespace Tactics
