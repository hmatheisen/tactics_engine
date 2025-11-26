#include <Tactics/Core/Grid.hpp>
#include <Tactics/Core/SQLiteGridRepository.hpp>
#include <Tactics/Core/Tile.hpp>
#include <catch2/catch_test_macros.hpp>
#include <filesystem>

TEST_CASE("SQLiteGridRepository - Save and Load Map", "[GridRepository]")
{
    // Use a temporary database for testing
    const std::string test_db = "test_maps.db";
    std::filesystem::remove(test_db); // Clean up any existing test DB

    Tactics::SQLiteGridRepository repository(test_db);

    SECTION("Save and load a simple grid")
    {
        Tactics::Grid grid;
        grid.resize(10, 10);

        // Set some tiles
        for (int y = 0; y < 10; ++y)
        {
            for (int x = 0; x < 10; ++x)
            {
                Tactics::Vector2i pos(x, y);
                Tactics::TileType type = (x + y) % 2 == 0 ? Tactics::TileType::Grass : Tactics::TileType::Water;
                Tactics::Tile tile(pos, type, 1);
                grid.set_tile(pos, tile);
            }
        }

        REQUIRE(repository.save_map("test_map", grid));

        auto loaded_grid_opt = repository.load_map("test_map");
        REQUIRE(loaded_grid_opt.has_value());

        const auto &loaded_grid = loaded_grid_opt.value();
        REQUIRE(loaded_grid.get_width() == 10);
        REQUIRE(loaded_grid.get_height() == 10);

        // Verify tiles
        for (int y = 0; y < 10; ++y)
        {
            for (int x = 0; x < 10; ++x)
            {
                Tactics::Vector2i pos(x, y);
                const Tactics::Tile *original = grid.get_tile(pos);
                const Tactics::Tile *loaded = loaded_grid.get_tile(pos);

                REQUIRE(original != nullptr);
                REQUIRE(loaded != nullptr);
                REQUIRE(original->get_type() == loaded->get_type());
                REQUIRE(original->get_move_cost() == loaded->get_move_cost());
            }
        }
    }

    SECTION("Save and load multiple maps")
    {
        Tactics::Grid grid1;
        grid1.resize(5, 5);
        Tactics::Grid grid2;
        grid2.resize(8, 8);

        REQUIRE(repository.save_map("map1", grid1));
        REQUIRE(repository.save_map("map2", grid2));

        auto loaded1 = repository.load_map("map1");
        auto loaded2 = repository.load_map("map2");

        REQUIRE(loaded1.has_value());
        REQUIRE(loaded2.has_value());
        REQUIRE(loaded1->get_width() == 5);
        REQUIRE(loaded1->get_height() == 5);
        REQUIRE(loaded2->get_width() == 8);
        REQUIRE(loaded2->get_height() == 8);
    }

    SECTION("List maps")
    {
        Tactics::Grid grid;
        grid.resize(3, 3);

        REQUIRE(repository.save_map("list_test_1", grid));
        REQUIRE(repository.save_map("list_test_2", grid));

        auto maps = repository.list_maps();
        REQUIRE(maps.size() >= 2);

        bool found1 = false;
        bool found2 = false;
        for (const auto &map : maps)
        {
            if (map.name == "list_test_1")
            {
                found1 = true;
            }
            if (map.name == "list_test_2")
            {
                found2 = true;
            }
        }
        REQUIRE(found1);
        REQUIRE(found2);
    }

    SECTION("Map exists")
    {
        Tactics::Grid grid;
        grid.resize(2, 2);

        REQUIRE(repository.save_map("exists_test", grid));
        REQUIRE(repository.map_exists("exists_test"));
        REQUIRE_FALSE(repository.map_exists("nonexistent"));
    }

    SECTION("Delete map")
    {
        Tactics::Grid grid;
        grid.resize(4, 4);

        REQUIRE(repository.save_map("delete_test", grid));
        REQUIRE(repository.map_exists("delete_test"));

        REQUIRE(repository.delete_map("delete_test"));
        REQUIRE_FALSE(repository.map_exists("delete_test"));
    }

    SECTION("Load non-existent map")
    {
        auto result = repository.load_map("nonexistent_map");
        REQUIRE_FALSE(result.has_value());
    }

    // Cleanup
    std::filesystem::remove(test_db);
}

TEST_CASE("SQLiteGridRepository - Large Map Performance", "[GridRepository][Performance]")
{
    const std::string test_db = "test_large_maps.db";
    std::filesystem::remove(test_db);

    Tactics::SQLiteGridRepository repository(test_db);

    SECTION("Save and load large map (256x256)")
    {
        Tactics::Grid grid;
        grid.resize(256, 256);

        // Fill with various tile types
        for (int y = 0; y < 256; ++y)
        {
            for (int x = 0; x < 256; ++x)
            {
                Tactics::Vector2i pos(x, y);
                Tactics::TileType type = static_cast<Tactics::TileType>((x + y) % 7);
                Tactics::Tile tile(pos, type, (x + y) % 5);
                grid.set_tile(pos, tile);
            }
        }

        REQUIRE(repository.save_map("large_map", grid));

        auto loaded_opt = repository.load_map("large_map");
        REQUIRE(loaded_opt.has_value());

        const auto &loaded = loaded_opt.value();
        REQUIRE(loaded.get_width() == 256);
        REQUIRE(loaded.get_height() == 256);

        // Verify a sample of tiles
        for (int i = 0; i < 100; ++i)
        {
            int x = i % 256;
            int y = i / 256;
            Tactics::Vector2i pos(x, y);
            const Tactics::Tile *original = grid.get_tile(pos);
            const Tactics::Tile *loaded_tile = loaded.get_tile(pos);

            REQUIRE(original != nullptr);
            REQUIRE(loaded_tile != nullptr);
            REQUIRE(original->get_type() == loaded_tile->get_type());
            REQUIRE(original->get_move_cost() == loaded_tile->get_move_cost());
        }
    }

    // Cleanup
    std::filesystem::remove(test_db);
}

