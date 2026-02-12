#pragma once

#include "Tactics/Components/Tile.hpp"
#include "Tactics/Core/Coordinates.hpp"

#include <optional>
#include <string>

namespace Tactics::Events
{
    struct CursorMoved
    {
        GridPos grid_position;
        WorldPos world_position;
    };

    struct UnitSelected
    {
        std::optional<std::size_t> unit_index;
    };

    struct UnitMoved
    {
        std::size_t unit_index{};
        GridPos from;
        GridPos to;
    };

    struct CameraChanged
    {
        WorldPos position{Vector2f{0.0F, 0.0F}};
        float zoom{1.0F};
    };

    struct MapLoaded
    {
        std::string map_name;
        int width{0};
        int height{0};
    };

    struct MapRegenerated
    {
        std::string map_name;
        int seed;
    };

    struct TileHovered
    {
        GridPos position{};
        Tile::Type type{};
    };
} // namespace Tactics::Events
