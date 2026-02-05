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

namespace
{
    constexpr float k_octave_persistence = 0.5F;
    constexpr float k_octave_lacunarity = 2.0F;
    constexpr float k_heightmap_min = 0.0F;
    constexpr float k_heightmap_max = 1.0F;
    constexpr float k_min_amplitude = 0.0001F;
    constexpr float k_selector_scale = 2.0F;
    constexpr float k_selector_offset_x = 17.0F;
    constexpr float k_selector_offset_y = 31.0F;
    constexpr float k_selector_threshold = 0.5F;
    constexpr float k_road_density = 0.03F;
    constexpr float k_hash_normalizer = 16777215.0F;
    constexpr float k_smoothstep_a = 3.0F;
    constexpr float k_smoothstep_b = 2.0F;

    constexpr int k_move_cost_walkable = 1;
    constexpr int k_move_cost_slow = 2;
    constexpr int k_move_cost_blocked = -1;
    constexpr int k_min_road_count = 1;
    constexpr int k_walkable_neighbor_threshold = 3;

    constexpr uint32_t k_hash_prime_x = 374761393U;
    constexpr uint32_t k_hash_prime_y = 668265263U;
    constexpr uint32_t k_hash_seed_mix = 0x9E3779B9U;
    constexpr uint32_t k_hash_shift_left = 6U;
    constexpr uint32_t k_hash_shift_right = 2U;
    constexpr uint32_t k_hash_xor_shift = 13U;
    constexpr uint32_t k_hash_multiplier = 1274126177U;
    constexpr uint32_t k_hash_final_shift = 16U;
    constexpr uint32_t k_hash_mask = 0x00FFFFFFU;
} // namespace

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

                int move_cost = k_move_cost_walkable;
                switch (tile_type)
                {
                case Tile::Type::Grass:
                case Tile::Type::Road:
                    move_cost = k_move_cost_walkable;
                    break;
                case Tile::Type::Desert:
                case Tile::Type::Forest:
                    move_cost = k_move_cost_slow;
                    break;
                case Tile::Type::Water:
                case Tile::Type::Mountain:
                case Tile::Type::Wall:
                    move_cost = k_move_cost_blocked;
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

                    amplitude *= k_octave_persistence;
                    frequency *= k_octave_lacunarity;
                }

                value = value / std::max(max_amplitude, k_min_amplitude);
                heightmap[index_of(x_pos, y_pos)] =
                    std::clamp(value, k_heightmap_min, k_heightmap_max);
            }
        }

        return heightmap;
    }

    auto MapGenerator::simple_noise(float x_pos, float y_pos) const -> float
    {
        const int x_floor = static_cast<int>(std::floor(x_pos));
        const int y_floor = static_cast<int>(std::floor(y_pos));
        const int x_next = x_floor + 1;
        const int y_next = y_floor + 1;

        const float x_weight = x_pos - static_cast<float>(x_floor);
        const float y_weight = y_pos - static_cast<float>(y_floor);

        const float value_00 = hash_noise(x_floor, y_floor);
        const float value_10 = hash_noise(x_next, y_floor);
        const float value_01 = hash_noise(x_floor, y_next);
        const float value_11 = hash_noise(x_next, y_next);

        const auto smoothstep = [](float factor) -> float
        { return factor * factor * (k_smoothstep_a - (k_smoothstep_b * factor)); };
        const auto lerp = [](float value_start, float value_end, float factor) -> float
        { return value_start + ((value_end - value_start) * factor); };

        const float interp_x0 = lerp(value_00, value_10, smoothstep(x_weight));
        const float interp_x1 = lerp(value_01, value_11, smoothstep(x_weight));
        return lerp(interp_x0, interp_x1, smoothstep(y_weight));
    }

    auto MapGenerator::hash_noise(int x_pos, int y_pos) const -> float
    {
        uint32_t hash_value = (static_cast<uint32_t>(x_pos) * k_hash_prime_x) +
                              (static_cast<uint32_t>(y_pos) * k_hash_prime_y);
        hash_value ^= static_cast<uint32_t>(m_config.seed) + k_hash_seed_mix +
                      (hash_value << k_hash_shift_left) + (hash_value >> k_hash_shift_right);
        hash_value = (hash_value ^ (hash_value >> k_hash_xor_shift)) * k_hash_multiplier;
        hash_value ^= hash_value >> k_hash_final_shift;
        return static_cast<float>(hash_value & k_hash_mask) / k_hash_normalizer;
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
                    const float selector = simple_noise(
                        (static_cast<float>(x_pos) * k_selector_scale) + k_selector_offset_x,
                        (static_cast<float>(y_pos) * k_selector_scale) + k_selector_offset_y);
                    tiles[idx] =
                        selector > k_selector_threshold ? Tile::Type::Mountain : Tile::Type::Desert;
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

                    const Vector2i position(x_pos, y_pos);
                    int water_neighbors = 0;
                    int grass_neighbors = 0;
                    int forest_neighbors = 0;
                    int mountain_neighbors = 0;
                    int desert_neighbors = 0;

                    water_neighbors = count_neighbors(tiles, position, Tile::Type::Water);
                    grass_neighbors = count_neighbors(tiles, position, Tile::Type::Grass);
                    forest_neighbors = count_neighbors(tiles, position, Tile::Type::Forest);
                    mountain_neighbors = count_neighbors(tiles, position, Tile::Type::Mountain);
                    desert_neighbors = count_neighbors(tiles, position, Tile::Type::Desert);

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
            const auto seed = static_cast<std::mt19937::result_type>(m_config.seed);
            std::mt19937 rng(seed);
            std::uniform_int_distribution<size_t> dist(0, grass_positions.size() - 1);

            const size_t road_count = std::max(
                static_cast<size_t>(k_min_road_count),
                static_cast<size_t>(static_cast<float>(grass_positions.size()) * k_road_density));

            for (size_t i = 0; i < road_count && i < grass_positions.size(); ++i)
            {
                const Vector2i pos = grass_positions[dist(rng)];
                grid.set_tile(pos, Tile(pos, Tile::Type::Road, k_move_cost_walkable));
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

                if (tile == nullptr || tile->is_walkable())
                {
                    continue;
                }

                const int walkable_neighbors = count_walkable_neighbors(grid, pos);
                if (walkable_neighbors >= k_walkable_neighbor_threshold)
                {
                    grid.set_tile(pos, Tile(pos, Tile::Type::Grass, k_move_cost_walkable));
                }
            }
        }
    }

    auto MapGenerator::index_of(int x_pos, int y_pos) const -> size_t
    {
        return (static_cast<size_t>(y_pos) * static_cast<size_t>(m_config.width)) +
               static_cast<size_t>(x_pos);
    }

    auto MapGenerator::count_neighbors(const std::vector<Tile::Type> &tiles, Vector2i position,
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

                const int nx_val = position.x + dx_val;
                const int ny_val = position.y + dy_val;

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

    auto MapGenerator::count_walkable_neighbors(const Grid &grid, Vector2i position) -> int
    {
        int walkable_neighbors = 0;
        const std::array<Vector2i, 4> offsets = {Vector2i(-1, 0), Vector2i(1, 0), Vector2i(0, -1),
                                                 Vector2i(0, 1)};

        for (const auto &offset : offsets)
        {
            const Vector2i neighbor_pos = position + offset;
            const Tile *neighbor = grid.get_tile(neighbor_pos);
            if (neighbor != nullptr && neighbor->is_walkable())
            {
                ++walkable_neighbors;
            }
        }

        return walkable_neighbors;
    }

    auto MapGenerator::flood_fill_count(const Grid &grid, Vector2i start) const -> int
    {
        std::unordered_set<size_t> visited;
        std::queue<Vector2i> to_visit;

        to_visit.push(start);
        visited.insert(index_of(start.x, start.y));

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

                const size_t hash = index_of(neighbor.x, neighbor.y);
                if (visited.contains(hash))
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
