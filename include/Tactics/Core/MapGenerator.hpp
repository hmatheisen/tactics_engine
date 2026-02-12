#pragma once

#include "Tactics/Components/Grid.hpp"
#include "Tactics/Components/Tile.hpp"
#include "Tactics/Core/GeneratorConfig.hpp"

#include <vector>

namespace Tactics
{
    // Procedural map generator using hybrid approach
    class MapGenerator
    {
    public:
        explicit MapGenerator(const GeneratorConfig &config);

        // Generate a new map
        [[nodiscard]] auto generate() -> Grid;

    private:
        // Stage 1: Generate base heightmap using simple noise
        [[nodiscard]] auto generate_heightmap() -> std::vector<float>;

        // Simple noise function (no external dependencies)
        [[nodiscard]] auto simple_noise(float x_pos, float y_pos) const -> float;

        // Stage 2: Convert heightmap to tile types
        [[nodiscard]] auto heightmap_to_tiles(const std::vector<float> &heightmap)
            -> std::vector<Tile::Type>;

        // Stage 3: Apply cellular automata smoothing
        auto apply_cellular_automata(std::vector<Tile::Type> &tiles) -> void;

        // Stage 4: Post-process for tactical features
        auto add_tactical_features(Grid &grid) -> void;

        // Utility: Check if all walkable tiles are connected
        auto ensure_connectivity(Grid &grid) -> void;

        // Helper: Convert 2D coordinates to 1D index
        [[nodiscard]] auto index_of(int x_pos, int y_pos) const -> size_t;

        // Helper: Count neighbors of a specific type
        [[nodiscard]] auto count_neighbors(const std::vector<Tile::Type> &tiles, Vector2i position,
                                           Tile::Type type) const -> int;

        // Helper: Flood fill to check connectivity
        [[nodiscard]] auto flood_fill_count(const Grid &grid, Vector2i start) const -> int;

        // Helper: Count walkable neighbors on a grid
        [[nodiscard]] static auto count_walkable_neighbors(const Grid &grid, Vector2i position)
            -> int;

        // Helper: Generate a deterministic hash value for noise
        [[nodiscard]] auto hash_noise(int x_pos, int y_pos) const -> float;

        GeneratorConfig m_config;
    };
} // namespace Tactics
