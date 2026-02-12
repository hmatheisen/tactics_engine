#pragma once

#include "Tactics/Components/Camera.hpp"
#include "Tactics/Core/GameConfig.hpp"
#include "Tactics/Core/Vector2.hpp"

namespace Tactics
{
    struct GridPos
    {
        Vector2i value{0, 0};
    };

    struct WorldPos
    {
        Vector2f value{0.0F, 0.0F};
    };

    struct ScreenPos
    {
        Vector2f value{0.0F, 0.0F};
    };

    [[nodiscard]] inline auto make_grid_pos(int x_pos, int y_pos) -> GridPos
    {
        return GridPos{Vector2i{x_pos, y_pos}};
    }

    [[nodiscard]] inline auto make_world_pos(float x_pos, float y_pos) -> WorldPos
    {
        return WorldPos{Vector2f{x_pos, y_pos}};
    }

    [[nodiscard]] inline auto make_screen_pos(float x_pos, float y_pos) -> ScreenPos
    {
        return ScreenPos{Vector2f{x_pos, y_pos}};
    }

    [[nodiscard]] inline auto to_world(const GridPos &grid_pos, const GameConfig &config)
        -> WorldPos
    {
        const float world_x = static_cast<float>(grid_pos.value.x) * config.tile_size;
        const float world_y = static_cast<float>(grid_pos.value.y) * config.tile_size;
        return WorldPos{Vector2f{world_x, world_y}};
    }

    [[nodiscard]] inline auto to_grid(const WorldPos &world_pos, const GameConfig &config)
        -> GridPos
    {
        const int grid_x = static_cast<int>(std::round(world_pos.value.x / config.tile_size));
        const int grid_y = static_cast<int>(std::round(world_pos.value.y / config.tile_size));
        return GridPos{Vector2i{grid_x, grid_y}};
    }

    [[nodiscard]] inline auto to_screen(const WorldPos &world_pos, const Camera &camera)
        -> ScreenPos
    {
        return ScreenPos{camera.world_to_screen(world_pos.value)};
    }

    [[nodiscard]] inline auto to_world(const ScreenPos &screen_pos, const Camera &camera)
        -> WorldPos
    {
        return WorldPos{camera.screen_to_world(screen_pos.value)};
    }
} // namespace Tactics
