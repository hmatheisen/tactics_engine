#include "Tactics/Core/MapGenerator.hpp"
#include "Tactics/Core/Logger.hpp"
#include "Tactics/Core/Tile.hpp"
#include "Tactics/Core/Vector2.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <queue>
#include <random>
#include <unordered_set>

namespace Tactics
{
    MapGenerator::MapGenerator(const GeneratorConfig &config) : m_config(config) {}

    auto MapGenerator::generate() -> Grid
    {
        log_info("Generating map: " + std::to_string(m_config.width) + "x" +
                 std::to_string(m_config.height) + " seed " + std::to_string(m_config.seed));

        // Stage 1: Generate heightmap using simple noise
        auto heightmap = generate_heightmap();

        // Stage 2: Convert heightmap to tile types
        auto tile_types = heightmap_to_tiles(heightmap);

        // Stage 3: Apply cellular automata smoothing
        apply_cellular_automata(tile_types);

        // Create grid and populate with tiles
        Grid grid;
        grid.resize(m_config.width, m_config.height);

        for (int y_pos = 0; y_pos < m_config.height; ++y_pos)
        {
            for (int x_pos = 0; x_pos < m_config.width; ++x_pos)
            {
                const size_t idx = index_of(x_pos, y_pos);
                const Tile::Type tile_type = tile_types[idx];

                int move_cost = 1;
                switch (tile_type)
                {
                case Tile::Type::Grass:
                    move_cost = 1;
                    break;
                case Tile::Type::Forest:
                    move_cost = 2;
                    break;
                case Tile::Type::Desert:
                    move_cost = 2;
                    break;
                case Tile::Type::Road:
                    move_cost = 1;
                    break;
                case Tile::Type::Water:
                case Tile::Type::Mountain:
                case Tile::Type::Wall:
                    move_cost = -1;
                    break;
                }

                const Vector2i position(x_pos, y_pos);
                const Tile tile(position, tile_type, move_cost);
                grid.set_tile(position, tile);
            }
        }

        // Stage 4: Post-process for tactical features
        add_tactical_features(grid);

        log_info("Map generation complete");
        return grid;
    }

    auto MapGenerator::generate_heightmap() -> std::vector<float>
    {
        std::vector<float> heightmap(static_cast<size_t>(m_config.width * m_config.height));

        for (int y_pos = 0; y_pos < m_config.height; ++y_pos)
        {
            for (int x_pos = 0; x_pos < m_config.width; ++x_pos)
            {
                float frequency = m_config.noise_scale;
                float amplitude = 1.0F;
                float value = 0.0F;
                float max_amplitude = 0.0F;

                for (int octave = 0; octave < m_config.noise_octaves; ++octave)
                {
                    const float sample_x = static_cast<float>(x_pos) * frequency;
                    const float sample_y = static_cast<float>(y_pos) * frequency;

                    value += simple_noise(sample_x, sample_y) * amplitude;
                    max_amplitude += amplitude;

                    amplitude *= 0.5F;
                    frequency *= 2.0F;
                }

                value = value / std::max(max_amplitude, 0.0001F);
                heightmap[index_of(x_pos, y_pos)] = std::clamp(value, 0.0F, 1.0F);
            }
        }

        return heightmap;
    }

    auto MapGenerator::simple_noise(float x_pos, float y_pos) const -> float
    {
        const int x0 = static_cast<int>(std::floor(x_pos));
        const int y0 = static_cast<int>(std::floor(y_pos));
        const int x1 = x0 + 1;
        const int y1 = y0 + 1;

        const float sx = x_pos - static_cast<float>(x0);
        const float sy = y_pos - static_cast<float>(y0);

        const float n00 = hash_noise(x0, y0);
        const float n10 = hash_noise(x1, y0);
        const float n01 = hash_noise(x0, y1);
        const float n11 = hash_noise(x1, y1);

        const auto smoothstep = [](float t) -> float { return t * t * (3.0F - 2.0F * t); };
        const auto lerp = [](float a, float b, float t) -> float { return a + (b - a) * t; };

        const float ix0 = lerp(n00, n10, smoothstep(sx));
        const float ix1 = lerp(n01, n11, smoothstep(sx));
        return lerp(ix0, ix1, smoothstep(sy));
    }

    auto MapGenerator::hash_noise(int x_pos, int y_pos) const -> float
    {
        uint32_t n = static_cast<uint32_t>(x_pos) * 374761393U +
                     static_cast<uint32_t>(y_pos) * 668265263U;
        n ^= static_cast<uint32_t>(m_config.seed) + 0x9E3779B9U + (n << 6U) + (n >> 2U);
        n = (n ^ (n >> 13U)) * 1274126177U;
        n ^= n >> 16U;
        return static_cast<float>(n & 0x00FFFFFFU) / 16777215.0F;
    }

    auto MapGenerator::heightmap_to_tiles(const std::vector<float> &heightmap)
        -> std::vector<Tile::Type>
    {
        std::vector<Tile::Type> tiles(heightmap.size());

        for (int y_pos = 0; y_pos < m_config.height; ++y_pos)
        {
            for (int x_pos = 0; x_pos < m_config.width; ++x_pos)
            {
                const size_t idx = index_of(x_pos, y_pos);
                const float height = heightmap[idx];

                if (height < m_config.water_threshold)
                {
                    tiles[idx] = Tile::Type::Water;
                }
                else if (height < m_config.grass_threshold)
                {
                    tiles[idx] = Tile::Type::Grass;
                }
                else if (height < m_config.forest_threshold)
                {
                    tiles[idx] = Tile::Type::Forest;
                }
                else if (height < m_config.mountain_threshold)
                {
                    const float selector = simple_noise(static_cast<float>(x_pos) * 2.0F + 17.0F,
                                                       static_cast<float>(y_pos) * 2.0F + 31.0F);
                    tiles[idx] = selector > 0.5F ? Tile::Type::Mountain : Tile::Type::Desert;
                }
                else
                {
                    tiles[idx] = Tile::Type::Mountain;
                }
            }
        }

        return tiles;
    }

    auto MapGenerator::apply_cellular_automata(std::vector<Tile::Type> &tiles) -> void
    {
        constexpr int MAJORITY_THRESHOLD = 5;

        for (int iteration = 0; iteration < m_config.ca_iterations; ++iteration)
        {
            std::vector<Tile::Type> new_tiles = tiles;

            for (int y_pos = 0; y_pos < m_config.height; ++y_pos)
            {
                for (int x_pos = 0; x_pos < m_config.width; ++x_pos)
                {
                    const size_t idx = index_of(x_pos, y_pos);

                    const int water_neighbors =
                        count_neighbors(tiles, x_pos, y_pos, Tile::Type::Water);
                    const int grass_neighbors =
                        count_neighbors(tiles, x_pos, y_pos, Tile::Type::Grass);
                    const int forest_neighbors =
                        count_neighbors(tiles, x_pos, y_pos, Tile::Type::Forest);
                    const int mountain_neighbors =
                        count_neighbors(tiles, x_pos, y_pos, Tile::Type::Mountain);
                    const int desert_neighbors =
                        count_neighbors(tiles, x_pos, y_pos, Tile::Type::Desert);

                    if (water_neighbors >= MAJORITY_THRESHOLD)
                    {
                        new_tiles[idx] = Tile::Type::Water;
                    }
                    else if (mountain_neighbors >= MAJORITY_THRESHOLD)
                    {
                        new_tiles[idx] = Tile::Type::Mountain;
                    }
                    else if (forest_neighbors >= MAJORITY_THRESHOLD)
                    {
                        new_tiles[idx] = Tile::Type::Forest;
                    }
                    else if (grass_neighbors >= MAJORITY_THRESHOLD)
                    {
                        new_tiles[idx] = Tile::Type::Grass;
                    }
                    else if (desert_neighbors >= MAJORITY_THRESHOLD)
                    {
                        new_tiles[idx] = Tile::Type::Desert;
                    }
                }
            }

            tiles = std::move(new_tiles);
        }
    }

    auto MapGenerator::add_tactical_features(Grid &grid) -> void
    {
        ensure_connectivity(grid);

        std::vector<Vector2i> grass_positions;
        for (int y_pos = 0; y_pos < m_config.height; ++y_pos)
        {
            for (int x_pos = 0; x_pos < m_config.width; ++x_pos)
            {
                const Vector2i pos(x_pos, y_pos);
                const Tile *tile = grid.get_tile(pos);
                if (tile != nullptr && tile->get_type() == Tile::Type::Grass)
                {
                    grass_positions.push_back(pos);
                }
            }
        }

        if (!grass_positions.empty())
        {
            std::mt19937 rng(static_cast<unsigned int>(m_config.seed));
            std::uniform_int_distribution<size_t> dist(0, grass_positions.size() - 1);

            const size_t road_count = std::max(static_cast<size_t>(1),
                                               static_cast<size_t>(grass_positions.size() * 0.03F));

            for (size_t i = 0; i < road_count && i < grass_positions.size(); ++i)
            {
                const Vector2i pos = grass_positions[dist(rng)];
                grid.set_tile(pos, Tile(pos, Tile::Type::Road, 1));
            }
        }
    }

    auto MapGenerator::ensure_connectivity(Grid &grid) -> void
    {
        std::vector<Vector2i> walkable_tiles;
        for (int y_pos = 0; y_pos < m_config.height; ++y_pos)
        {
            for (int x_pos = 0; x_pos < m_config.width; ++x_pos)
            {
                const Vector2i pos(x_pos, y_pos);
                const Tile *tile = grid.get_tile(pos);
                if (tile != nullptr && tile->is_walkable())
                {
                    walkable_tiles.push_back(pos);
                }
            }
        }

        if (walkable_tiles.empty())
        {
            log_warning("No walkable tiles found in generated map");
            return;
        }

        const int connected_count = flood_fill_count(grid, walkable_tiles[0]);
        if (connected_count >= static_cast<int>(walkable_tiles.size()))
        {
            return;
        }

        for (int y_pos = 1; y_pos < m_config.height - 1; ++y_pos)
        {
            for (int x_pos = 1; x_pos < m_config.width - 1; ++x_pos)
            {
                const Vector2i pos(x_pos, y_pos);
                const Tile *tile = grid.get_tile(pos);

                if (tile != nullptr && !tile->is_walkable())
                {
                    int walkable_neighbors = 0;
                    const std::array<Vector2i, 4> offsets = {
                        Vector2i(-1, 0), Vector2i(1, 0), Vector2i(0, -1), Vector2i(0, 1)};

                    for (const auto &offset : offsets)
                    {
                        const Vector2i neighbor_pos = pos + offset;
                        const Tile *neighbor = grid.get_tile(neighbor_pos);
                        if (neighbor != nullptr && neighbor->is_walkable())
                        {
                            ++walkable_neighbors;
                        }
                    }

                    if (walkable_neighbors >= 3)
                    {
                        grid.set_tile(pos, Tile(pos, Tile::Type::Grass, 1));
                    }
                }
            }
        }
    }

    auto MapGenerator::index_of(int x_pos, int y_pos) const -> size_t
    {
        return static_cast<size_t>(y_pos * m_config.width + x_pos);
    }

    auto MapGenerator::count_neighbors(const std::vector<Tile::Type> &tiles, int x_pos, int y_pos,
                                       Tile::Type type) const -> int
    {
        int count = 0;

        for (int dy_val = -1; dy_val <= 1; ++dy_val)
        {
            for (int dx_val = -1; dx_val <= 1; ++dx_val)
            {
                if (dx_val == 0 && dy_val == 0)
                {
                    continue;
                }

                const int nx_val = x_pos + dx_val;
                const int ny_val = y_pos + dy_val;

                if (nx_val >= 0 && nx_val < m_config.width && ny_val >= 0 &&
                    ny_val < m_config.height)
                {
                    const size_t idx = index_of(nx_val, ny_val);
                    if (tiles[idx] == type)
                    {
                        ++count;
                    }
                }
            }
        }

        return count;
    }

    auto MapGenerator::flood_fill_count(const Grid &grid, Vector2i start) const -> int
    {
        std::unordered_set<int> visited;
        std::queue<Vector2i> to_visit;

        to_visit.push(start);
        visited.insert(start.y * m_config.width + start.x);

        int count = 0;

        while (!to_visit.empty())
        {
            const Vector2i current = to_visit.front();
            to_visit.pop();
            ++count;

            const std::array<Vector2i, 4> offsets = {Vector2i(-1, 0), Vector2i(1, 0),
                                                     Vector2i(0, -1), Vector2i(0, 1)};

            for (const auto &offset : offsets)
            {
                const Vector2i neighbor = current + offset;

                if (neighbor.x < 0 || neighbor.x >= m_config.width || neighbor.y < 0 ||
                    neighbor.y >= m_config.height)
                {
                    continue;
                }

                const int hash = neighbor.y * m_config.width + neighbor.x;
                if (visited.find(hash) != visited.end())
                {
                    continue;
                }

                const Tile *tile = grid.get_tile(neighbor);
                if (tile != nullptr && tile->is_walkable())
                {
                    visited.insert(hash);
                    to_visit.push(neighbor);
                }
            }
        }

        return count;
    }
} // namespace Tactics
