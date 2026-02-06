#include "Tactics/Core/MapGenerator.hpp"
#include "Tactics/Components/Grid.hpp"
#include "Tactics/Components/Tile.hpp"
#include "Tactics/Core/Vector2.hpp"
#include <catch2/catch_test_macros.hpp>
#include <array>
#include <queue>
#include <unordered_set>

// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cppcoreguidelines-avoid-magic-numbers,readability-function-cognitive-complexity,readability-identifier-length,readability-magic-numbers)
namespace
{
    auto count_connected_tiles(const Tactics::Grid &grid, Tactics::Vector2i start) -> int
    {
        const int width = grid.get_width();
        const int height = grid.get_height();

        std::unordered_set<int> visited;
        std::queue<Tactics::Vector2i> to_visit;

        to_visit.push(start);
        visited.insert((start.y * width) + start.x);

        int count = 0;

        while (!to_visit.empty())
        {
            const Tactics::Vector2i current = to_visit.front();
            to_visit.pop();
            ++count;

            const std::array<Tactics::Vector2i, 4> offsets = {Tactics::Vector2i(-1, 0),
                                                               Tactics::Vector2i(1, 0),
                                                               Tactics::Vector2i(0, -1),
                                                               Tactics::Vector2i(0, 1)};

            for (const auto &offset : offsets)
            {
                const Tactics::Vector2i neighbor = current + offset;
                if (neighbor.x < 0 || neighbor.x >= width || neighbor.y < 0 || neighbor.y >= height)
                {
                    continue;
                }

                const int hash = (neighbor.y * width) + neighbor.x;
                if (visited.find(hash) != visited.end())
                {
                    continue;
                }

                const Tactics::Tile *tile = grid.get_tile(neighbor);
                if (tile != nullptr && tile->is_walkable())
                {
                    visited.insert(hash);
                    to_visit.push(neighbor);
                }
            }
        }

        return count;
    }
} // namespace

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
TEST_CASE("MapGenerator - Basic Generation", "[MapGenerator]")
{
    SECTION("Generated map has correct dimensions")
    {
        Tactics::GeneratorConfig config = Tactics::GeneratorConfig::default_config();
        config.width = 30;
        config.height = 25;

        Tactics::MapGenerator generator(config);
        Tactics::Grid grid = generator.generate();

        REQUIRE(grid.get_width() == 30);
        REQUIRE(grid.get_height() == 25);
    }

    SECTION("All tiles are valid types")
    {
        Tactics::GeneratorConfig config = Tactics::GeneratorConfig::default_config();
        config.width = 20;
        config.height = 20;

        Tactics::MapGenerator generator(config);
        Tactics::Grid grid = generator.generate();

        for (int y = 0; y < config.height; ++y)
        {
            for (int x = 0; x < config.width; ++x)
            {
                const Tactics::Vector2i pos(x, y);
                const Tactics::Tile *tile = grid.get_tile(pos);

                REQUIRE(tile != nullptr);

                const auto type = tile->get_type();
                REQUIRE((type == Tactics::Tile::Type::Grass || type == Tactics::Tile::Type::Water ||
                         type == Tactics::Tile::Type::Mountain ||
                         type == Tactics::Tile::Type::Forest || type == Tactics::Tile::Type::Desert ||
                         type == Tactics::Tile::Type::Road || type == Tactics::Tile::Type::Wall));
            }
        }
    }

    SECTION("Map contains walkable tiles")
    {
        Tactics::GeneratorConfig config = Tactics::GeneratorConfig::default_config();
        config.width = 20;
        config.height = 20;

        Tactics::MapGenerator generator(config);
        Tactics::Grid grid = generator.generate();

        int walkable_count = 0;
        for (int y = 0; y < config.height; ++y)
        {
            for (int x = 0; x < config.width; ++x)
            {
                const Tactics::Vector2i pos(x, y);
                const Tactics::Tile *tile = grid.get_tile(pos);
                if (tile != nullptr && tile->is_walkable())
                {
                    ++walkable_count;
                }
            }
        }

        REQUIRE(walkable_count > 0);
        REQUIRE(walkable_count >= (config.width * config.height) / 3);
    }
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
TEST_CASE("MapGenerator - Determinism", "[MapGenerator]")
{
    SECTION("Same seed produces identical maps")
    {
        Tactics::GeneratorConfig config = Tactics::GeneratorConfig::default_config();
        config.width = 15;
        config.height = 15;
        config.seed = 12345;

        Tactics::MapGenerator generator1(config);
        Tactics::Grid grid1 = generator1.generate();

        Tactics::MapGenerator generator2(config);
        Tactics::Grid grid2 = generator2.generate();

        for (int y = 0; y < config.height; ++y)
        {
            for (int x = 0; x < config.width; ++x)
            {
                const Tactics::Vector2i pos(x, y);
                const Tactics::Tile *tile1 = grid1.get_tile(pos);
                const Tactics::Tile *tile2 = grid2.get_tile(pos);

                REQUIRE(tile1 != nullptr);
                REQUIRE(tile2 != nullptr);
                REQUIRE(tile1->get_type() == tile2->get_type());
                REQUIRE(tile1->get_move_cost() == tile2->get_move_cost());
            }
        }
    }

    SECTION("Different seeds produce different maps")
    {
        Tactics::GeneratorConfig config1 = Tactics::GeneratorConfig::default_config();
        config1.width = 15;
        config1.height = 15;
        config1.seed = 12345;

        Tactics::GeneratorConfig config2 = config1;
        config2.seed = 54321;

        Tactics::MapGenerator generator1(config1);
        Tactics::Grid grid1 = generator1.generate();

        Tactics::MapGenerator generator2(config2);
        Tactics::Grid grid2 = generator2.generate();

        int different_tiles = 0;
        for (int y = 0; y < config1.height; ++y)
        {
            for (int x = 0; x < config1.width; ++x)
            {
                const Tactics::Vector2i pos(x, y);
                const Tactics::Tile *tile1 = grid1.get_tile(pos);
                const Tactics::Tile *tile2 = grid2.get_tile(pos);

                if (tile1 != nullptr && tile2 != nullptr &&
                    tile1->get_type() != tile2->get_type())
                {
                    ++different_tiles;
                }
            }
        }

        REQUIRE(different_tiles >= (config1.width * config1.height) / 5);
    }
}

TEST_CASE("MapGenerator - Connectivity", "[MapGenerator]")
{
    Tactics::GeneratorConfig config = Tactics::GeneratorConfig::default_config();
    config.width = 25;
    config.height = 25;
    config.seed = 42;

    Tactics::MapGenerator generator(config);
    Tactics::Grid grid = generator.generate();

    Tactics::Vector2i first_walkable(-1, -1);
    int total_walkable = 0;

    for (int y = 0; y < config.height && first_walkable.x == -1; ++y)
    {
        for (int x = 0; x < config.width; ++x)
        {
            const Tactics::Vector2i pos(x, y);
            const Tactics::Tile *tile = grid.get_tile(pos);
            if (tile != nullptr && tile->is_walkable())
            {
                if (first_walkable.x == -1)
                {
                    first_walkable = pos;
                }
                ++total_walkable;
            }
        }
    }

    REQUIRE(first_walkable.x != -1);

    const int connected_count = count_connected_tiles(grid, first_walkable);
    REQUIRE(connected_count == total_walkable);
}

// NOLINTEND(cppcoreguidelines-avoid-do-while,cppcoreguidelines-avoid-magic-numbers,readability-function-cognitive-complexity,readability-identifier-length,readability-magic-numbers)
