#include "Tactics/Core/SQLiteGridRepository.hpp"
#include "Tactics/Core/Logger.hpp"
#include "Tactics/Core/Tile.hpp"
#include <cstring>

namespace Tactics
{
    SQLiteGridRepository::SQLiteGridRepository(const std::string &db_path) : m_db(nullptr)
    {
        const int result = sqlite3_open(db_path.c_str(), &m_db);
        if (result != SQLITE_OK)
        {
            log_error("Failed to open SQLite database: " + db_path + " - " + sqlite3_errmsg(m_db));
            if (m_db != nullptr)
            {
                sqlite3_close(m_db);
                m_db = nullptr;
            }
            return;
        }

        if (!initialize_schema())
        {
            log_error("Failed to initialize database schema");
            if (m_db != nullptr)
            {
                sqlite3_close(m_db);
                m_db = nullptr;
            }
        }
    }

    SQLiteGridRepository::~SQLiteGridRepository()
    {
        if (m_db != nullptr)
        {
            sqlite3_close(m_db);
            m_db = nullptr;
        }
    }

    SQLiteGridRepository::SQLiteGridRepository(SQLiteGridRepository &&other) noexcept
        : m_db(other.m_db)
    {
        other.m_db = nullptr;
    }

    auto SQLiteGridRepository::operator=(SQLiteGridRepository &&other) noexcept
        -> SQLiteGridRepository &
    {
        if (this != &other)
        {
            if (m_db != nullptr)
            {
                sqlite3_close(m_db);
            }
            m_db = other.m_db;
            other.m_db = nullptr;
        }
        return *this;
    }

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    auto SQLiteGridRepository::initialize_schema() -> bool
    {
        if (m_db == nullptr)
        {
            return false;
        }

        // Create maps table
        const std::string create_maps_sql = R"(
            CREATE TABLE IF NOT EXISTS maps (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT NOT NULL UNIQUE,
                width INTEGER NOT NULL,
                height INTEGER NOT NULL,
                created_at TEXT NOT NULL DEFAULT (datetime('now')),
                updated_at TEXT NOT NULL DEFAULT (datetime('now'))
            )
        )";

        if (!execute_statement(create_maps_sql))
        {
            return false;
        }

        // Create tiles table
        // Note: sprite_id and variant columns are reserved for future graphical assets
        // They are nullable to maintain backward compatibility
        const std::string create_tiles_sql = R"(
            CREATE TABLE IF NOT EXISTS tiles (
                map_id INTEGER NOT NULL,
                x INTEGER NOT NULL,
                y INTEGER NOT NULL,
                tile_type INTEGER NOT NULL,
                move_cost INTEGER NOT NULL,
                sprite_id INTEGER,
                variant INTEGER,
                PRIMARY KEY (map_id, x, y),
                FOREIGN KEY (map_id) REFERENCES maps(id) ON DELETE CASCADE
            )
        )";

        if (!execute_statement(create_tiles_sql))
        {
            return false;
        }

        // Create generator configs table
        const std::string create_configs_sql = R"(
            CREATE TABLE IF NOT EXISTS generator_configs (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                map_name TEXT NOT NULL UNIQUE,
                seed INTEGER NOT NULL,
                noise_scale REAL NOT NULL DEFAULT 0.05,
                noise_octaves INTEGER NOT NULL DEFAULT 4,
                ca_iterations INTEGER NOT NULL DEFAULT 3,
                water_threshold REAL NOT NULL DEFAULT 0.3,
                grass_threshold REAL NOT NULL DEFAULT 0.5,
                forest_threshold REAL NOT NULL DEFAULT 0.7,
                mountain_threshold REAL NOT NULL DEFAULT 0.85,
                FOREIGN KEY (map_name) REFERENCES maps(name) ON DELETE CASCADE
            )
        )";

        if (!execute_statement(create_configs_sql))
        {
            return false;
        }

        // Create index for efficient tile lookups
        const std::string create_index_sql = R"(
            CREATE INDEX IF NOT EXISTS idx_tiles_map_position ON tiles(map_id, x, y)
        )";

        if (!execute_statement(create_index_sql))
        {
            return false;
        }

        log_info("Database schema initialized successfully");
        return true;
    }

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    auto SQLiteGridRepository::execute_statement(const std::string &sql) -> bool
    {
        if (m_db == nullptr)
        {
            return false;
        }

        char *error_message = nullptr;
        const int result = sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &error_message);

        if (result != SQLITE_OK)
        {
            log_error("SQLite error: " +
                      std::string(error_message != nullptr ? error_message : "Unknown error"));
            sqlite3_free(error_message);
            return false;
        }

        return true;
    }

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    auto SQLiteGridRepository::get_map_id(const std::string &map_name) -> std::optional<int>
    {
        if (m_db == nullptr)
        {
            return std::nullopt;
        }

        const std::string sql = "SELECT id FROM maps WHERE name = ?";
        sqlite3_stmt *stmt = nullptr;

        if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        {
            log_error("Failed to prepare statement: " + std::string(sqlite3_errmsg(m_db)));
            return std::nullopt;
        }

        if (sqlite3_bind_text(stmt, 1, map_name.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)
        {
            log_error("Failed to bind parameter: " + std::string(sqlite3_errmsg(m_db)));
            sqlite3_finalize(stmt);
            return std::nullopt;
        }

        std::optional<int> map_id = std::nullopt;
        const int result = sqlite3_step(stmt);

        if (result == SQLITE_ROW)
        {
            map_id = sqlite3_column_int(stmt, 0);
        }
        else if (result != SQLITE_DONE)
        {
            log_error("Failed to execute query: " + std::string(sqlite3_errmsg(m_db)));
        }

        sqlite3_finalize(stmt);
        return map_id;
    }

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    auto SQLiteGridRepository::upsert_map_metadata(const std::string &map_name, Vector2i size)
        -> std::optional<int>
    {
        if (m_db == nullptr)
        {
            return std::nullopt;
        }

        // Check if map exists
        const auto existing_id = get_map_id(map_name);

        if (existing_id.has_value())
        {
            // Update existing map
            const std::string update_sql =
                "UPDATE maps SET width = ?, height = ?, updated_at = datetime('now') "
                "WHERE id = ?";
            sqlite3_stmt *stmt = nullptr;

            if (sqlite3_prepare_v2(m_db, update_sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
            {
                log_error("Failed to prepare update statement: " +
                          std::string(sqlite3_errmsg(m_db)));
                return std::nullopt;
            }

            sqlite3_bind_int(stmt, 1, size.x);
            sqlite3_bind_int(stmt, 2, size.y);
            sqlite3_bind_int(stmt, 3, existing_id.value());

            if (sqlite3_step(stmt) != SQLITE_DONE)
            {
                log_error("Failed to update map metadata: " + std::string(sqlite3_errmsg(m_db)));
                sqlite3_finalize(stmt);
                return std::nullopt;
            }

            sqlite3_finalize(stmt);
            return existing_id;
        }

        // Insert new map
        const std::string insert_sql = "INSERT INTO maps (name, width, height) VALUES (?, ?, ?)";
        sqlite3_stmt *stmt = nullptr;

        if (sqlite3_prepare_v2(m_db, insert_sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        {
            log_error("Failed to prepare insert statement: " + std::string(sqlite3_errmsg(m_db)));
            return std::nullopt;
        }

        sqlite3_bind_text(stmt, 1, map_name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, size.x);
        sqlite3_bind_int(stmt, 3, size.y);

        if (sqlite3_step(stmt) != SQLITE_DONE)
        {
            log_error("Failed to insert map metadata: " + std::string(sqlite3_errmsg(m_db)));
            sqlite3_finalize(stmt);
            return std::nullopt;
        }

        const int new_id = static_cast<int>(sqlite3_last_insert_rowid(m_db));
        sqlite3_finalize(stmt);
        return new_id;
    }

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    auto SQLiteGridRepository::load_map(const std::string &map_name) -> std::optional<Grid>
    {
        if (m_db == nullptr)
        {
            log_error("Database connection is null");
            return std::nullopt;
        }

        const auto map_id = get_map_id(map_name);
        if (!map_id.has_value())
        {
            log_error("Map not found: " + map_name);
            return std::nullopt;
        }

        // Load map metadata
        const std::string metadata_sql = "SELECT width, height FROM maps WHERE id = ?";
        sqlite3_stmt *metadata_stmt = nullptr;

        if (sqlite3_prepare_v2(m_db, metadata_sql.c_str(), -1, &metadata_stmt, nullptr) !=
            SQLITE_OK)
        {
            log_error("Failed to prepare metadata query: " + std::string(sqlite3_errmsg(m_db)));
            return std::nullopt;
        }

        sqlite3_bind_int(metadata_stmt, 1, map_id.value());

        int width = 0;
        int height = 0;

        if (sqlite3_step(metadata_stmt) == SQLITE_ROW)
        {
            width = sqlite3_column_int(metadata_stmt, 0);
            height = sqlite3_column_int(metadata_stmt, 1);
        }
        else
        {
            log_error("Failed to load map metadata");
            sqlite3_finalize(metadata_stmt);
            return std::nullopt;
        }

        sqlite3_finalize(metadata_stmt);

        // Create grid with proper dimensions
        Grid grid;
        grid.resize(width, height);

        // Load tiles
        const std::string tiles_sql =
            "SELECT x, y, tile_type, move_cost FROM tiles WHERE map_id = ? ORDER BY y, x";
        sqlite3_stmt *tiles_stmt = nullptr;

        if (sqlite3_prepare_v2(m_db, tiles_sql.c_str(), -1, &tiles_stmt, nullptr) != SQLITE_OK)
        {
            log_error("Failed to prepare tiles query: " + std::string(sqlite3_errmsg(m_db)));
            return std::nullopt;
        }

        sqlite3_bind_int(tiles_stmt, 1, map_id.value());

        while (sqlite3_step(tiles_stmt) == SQLITE_ROW)
        {
            const int x_pos = sqlite3_column_int(tiles_stmt, 0);
            const int y_pos = sqlite3_column_int(tiles_stmt, 1);
            const int tile_type_int = sqlite3_column_int(tiles_stmt, 2);
            const int move_cost = sqlite3_column_int(tiles_stmt, 3);

            const auto tile_type = static_cast<Tile::Type>(tile_type_int);
            const Vector2i position(x_pos, y_pos);
            const Tile tile(position, tile_type, move_cost);

            grid.set_tile(position, tile);
        }

        sqlite3_finalize(tiles_stmt);

        log_info("Loaded map: " + map_name + " (" + std::to_string(width) + "x" +
                 std::to_string(height) + ")");
        return grid;
    }

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    auto SQLiteGridRepository::save_map(const std::string &map_name, const Grid &grid) -> bool
    {
        constexpr int STMT_MAP_ID = 1;
        constexpr int STMT_X_POS = 2;
        constexpr int STMT_Y_POS = 3;
        constexpr int STMT_TILE_TYPE = 4;
        constexpr int STMT_TILE_MOVE_COST = 5;

        if (m_db == nullptr)
        {
            log_error("Database connection is null");
            return false;
        }

        const int width = grid.get_width();
        const int height = grid.get_height();
        const Vector2i map_size(width, height);

        // Upsert map metadata
        const auto map_id = upsert_map_metadata(map_name, map_size);
        if (!map_id.has_value())
        {
            log_error("Failed to save map metadata");
            return false;
        }

        // Begin transaction for efficient bulk insert
        if (!execute_statement("BEGIN TRANSACTION"))
        {
            return false;
        }

        // Delete existing tiles for this map
        const std::string delete_sql = "DELETE FROM tiles WHERE map_id = ?";
        sqlite3_stmt *delete_stmt = nullptr;

        if (sqlite3_prepare_v2(m_db, delete_sql.c_str(), -1, &delete_stmt, nullptr) != SQLITE_OK)
        {
            log_error("Failed to prepare delete statement: " + std::string(sqlite3_errmsg(m_db)));
            execute_statement("ROLLBACK");
            return false;
        }

        sqlite3_bind_int(delete_stmt, 1, map_id.value());

        if (sqlite3_step(delete_stmt) != SQLITE_DONE)
        {
            log_error("Failed to delete existing tiles: " + std::string(sqlite3_errmsg(m_db)));
            sqlite3_finalize(delete_stmt);
            execute_statement("ROLLBACK");
            return false;
        }

        sqlite3_finalize(delete_stmt);

        // Insert all tiles
        // Note: sprite_id and variant are set to NULL for now (reserved for future graphics)
        const std::string insert_sql = "INSERT INTO tiles (map_id, x, y, tile_type, move_cost, "
                                       "sprite_id, variant) VALUES (?, ?, ?, ?, ?, NULL, NULL)";
        sqlite3_stmt *insert_stmt = nullptr;

        if (sqlite3_prepare_v2(m_db, insert_sql.c_str(), -1, &insert_stmt, nullptr) != SQLITE_OK)
        {
            log_error("Failed to prepare insert statement: " + std::string(sqlite3_errmsg(m_db)));
            execute_statement("ROLLBACK");
            return false;
        }

        for (int y_pos = 0; y_pos < height; ++y_pos)
        {
            for (int x_pos = 0; x_pos < width; ++x_pos)
            {
                const Vector2i position(x_pos, y_pos);
                const Tile *tile = grid.get_tile(position);

                if (tile == nullptr)
                {
                    continue;
                }

                sqlite3_reset(insert_stmt);
                sqlite3_bind_int(insert_stmt, STMT_MAP_ID, map_id.value());
                sqlite3_bind_int(insert_stmt, STMT_X_POS, x_pos);
                sqlite3_bind_int(insert_stmt, STMT_Y_POS, y_pos);
                sqlite3_bind_int(insert_stmt, STMT_TILE_TYPE, static_cast<int>(tile->get_type()));
                sqlite3_bind_int(insert_stmt, STMT_TILE_MOVE_COST, tile->get_move_cost());

                if (sqlite3_step(insert_stmt) != SQLITE_DONE)
                {
                    log_error("Failed to insert tile: " + std::string(sqlite3_errmsg(m_db)));
                    sqlite3_finalize(insert_stmt);
                    execute_statement("ROLLBACK");
                    return false;
                }
            }
        }

        sqlite3_finalize(insert_stmt);

        // Commit transaction
        if (!execute_statement("COMMIT"))
        {
            execute_statement("ROLLBACK");
            return false;
        }

        log_info("Saved map: " + map_name + " (" + std::to_string(width) + "x" +
                 std::to_string(height) + ")");
        return true;
    }

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    auto SQLiteGridRepository::list_maps() -> std::vector<MapMetadata>
    {
        constexpr int STMT_MAP_ID = 0;
        constexpr int STMT_MAP_NAME = 1;
        constexpr int STMT_MAP_WIDTH = 2;
        constexpr int STMT_MAP_HEIGHT = 3;
        constexpr int STMT_MAP_CREATED_AT = 4;
        constexpr int STMT_MAP_UPDATED_AT = 5;

        std::vector<MapMetadata> maps;

        if (m_db == nullptr)
        {
            return maps;
        }

        const std::string sql =
            "SELECT id, name, width, height, created_at, updated_at FROM maps ORDER BY name";
        sqlite3_stmt *stmt = nullptr;

        if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        {
            log_error("Failed to prepare list query: " + std::string(sqlite3_errmsg(m_db)));
            return maps;
        }

        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            MapMetadata metadata;

            // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
            metadata.id = sqlite3_column_int(stmt, STMT_MAP_ID);
            metadata.name =
                reinterpret_cast<const char *>(sqlite3_column_text(stmt, STMT_MAP_NAME));
            metadata.width = sqlite3_column_int(stmt, STMT_MAP_WIDTH);
            metadata.height = sqlite3_column_int(stmt, STMT_MAP_HEIGHT);
            metadata.created_at =
                reinterpret_cast<const char *>(sqlite3_column_text(stmt, STMT_MAP_CREATED_AT));
            metadata.updated_at =
                reinterpret_cast<const char *>(sqlite3_column_text(stmt, STMT_MAP_UPDATED_AT));
            // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)

            maps.push_back(metadata);
        }

        sqlite3_finalize(stmt);
        return maps;
    }

    auto SQLiteGridRepository::map_exists(const std::string &map_name) -> bool
    {
        return get_map_id(map_name).has_value();
    }

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    auto SQLiteGridRepository::delete_map(const std::string &map_name) -> bool
    {
        if (m_db == nullptr)
        {
            return false;
        }

        const auto map_id = get_map_id(map_name);
        if (!map_id.has_value())
        {
            log_error("Map not found: " + map_name);
            return false;
        }

        // Delete map (cascade will delete tiles)
        const std::string delete_sql = "DELETE FROM maps WHERE id = ?";
        sqlite3_stmt *stmt = nullptr;

        if (sqlite3_prepare_v2(m_db, delete_sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        {
            log_error("Failed to prepare delete statement: " + std::string(sqlite3_errmsg(m_db)));
            return false;
        }

        sqlite3_bind_int(stmt, 1, map_id.value());

        bool success = false;
        success = sqlite3_step(stmt) == SQLITE_DONE;
        if (!success)
        {
            log_error("Failed to delete map: " + std::string(sqlite3_errmsg(m_db)));
        }

        sqlite3_finalize(stmt);
        return success;
    }

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    auto SQLiteGridRepository::load_generator_config(const std::string &map_name)
        -> std::optional<GeneratorConfig>
    {
        if (m_db == nullptr)
        {
            return std::nullopt;
        }

        constexpr int COLUMN_WIDTH = 0;
        constexpr int COLUMN_HEIGHT = 1;
        constexpr int COLUMN_SEED = 2;
        constexpr int COLUMN_NOISE_SCALE = 3;
        constexpr int COLUMN_NOISE_OCTAVES = 4;
        constexpr int COLUMN_CA_ITERATIONS = 5;
        constexpr int COLUMN_WATER_THRESHOLD = 6;
        constexpr int COLUMN_GRASS_THRESHOLD = 7;
        constexpr int COLUMN_FOREST_THRESHOLD = 8;
        constexpr int COLUMN_MOUNTAIN_THRESHOLD = 9;

        const std::string sql = R"(
            SELECT m.width, m.height,
                   g.seed, g.noise_scale, g.noise_octaves, g.ca_iterations,
                   g.water_threshold, g.grass_threshold, g.forest_threshold, g.mountain_threshold
            FROM maps m
            LEFT JOIN generator_configs g ON g.map_name = m.name
            WHERE m.name = ?
        )";

        sqlite3_stmt *stmt = nullptr;
        if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        {
            log_error("Failed to prepare generator config query: " +
                      std::string(sqlite3_errmsg(m_db)));
            return std::nullopt;
        }

        sqlite3_bind_text(stmt, 1, map_name.c_str(), -1, SQLITE_STATIC);

        std::optional<GeneratorConfig> config = std::nullopt;
        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            if (sqlite3_column_type(stmt, COLUMN_SEED) != SQLITE_NULL)
            {
                GeneratorConfig loaded_config{};
                loaded_config.width = sqlite3_column_int(stmt, COLUMN_WIDTH);
                loaded_config.height = sqlite3_column_int(stmt, COLUMN_HEIGHT);
                loaded_config.seed = sqlite3_column_int(stmt, COLUMN_SEED);
                loaded_config.noise_scale =
                    static_cast<float>(sqlite3_column_double(stmt, COLUMN_NOISE_SCALE));
                loaded_config.noise_octaves = sqlite3_column_int(stmt, COLUMN_NOISE_OCTAVES);
                loaded_config.ca_iterations = sqlite3_column_int(stmt, COLUMN_CA_ITERATIONS);
                loaded_config.water_threshold =
                    static_cast<float>(sqlite3_column_double(stmt, COLUMN_WATER_THRESHOLD));
                loaded_config.grass_threshold =
                    static_cast<float>(sqlite3_column_double(stmt, COLUMN_GRASS_THRESHOLD));
                loaded_config.forest_threshold =
                    static_cast<float>(sqlite3_column_double(stmt, COLUMN_FOREST_THRESHOLD));
                loaded_config.mountain_threshold =
                    static_cast<float>(sqlite3_column_double(stmt, COLUMN_MOUNTAIN_THRESHOLD));

                config = loaded_config;
            }
        }

        sqlite3_finalize(stmt);
        return config;
    }

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    auto SQLiteGridRepository::save_generator_config(const std::string &map_name,
                                                     const GeneratorConfig &config) -> bool
    {
        if (m_db == nullptr)
        {
            return false;
        }

        const std::string sql = R"(
            INSERT INTO generator_configs (
                map_name, seed, noise_scale, noise_octaves, ca_iterations,
                water_threshold, grass_threshold, forest_threshold, mountain_threshold
            ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)
            ON CONFLICT(map_name) DO UPDATE SET
                seed = excluded.seed,
                noise_scale = excluded.noise_scale,
                noise_octaves = excluded.noise_octaves,
                ca_iterations = excluded.ca_iterations,
                water_threshold = excluded.water_threshold,
                grass_threshold = excluded.grass_threshold,
                forest_threshold = excluded.forest_threshold,
                mountain_threshold = excluded.mountain_threshold
        )";

        sqlite3_stmt *stmt = nullptr;
        if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        {
            log_error("Failed to prepare generator config upsert: " +
                      std::string(sqlite3_errmsg(m_db)));
            return false;
        }

        constexpr int STMT_MAP_NAME = 1;
        constexpr int STMT_SEED = 2;
        constexpr int STMT_NOISE_SCALE = 3;
        constexpr int STMT_NOISE_OCTAVES = 4;
        constexpr int STMT_CA_ITERATIONS = 5;
        constexpr int STMT_WATER_THRESHOLD = 6;
        constexpr int STMT_GRASS_THRESHOLD = 7;
        constexpr int STMT_FOREST_THRESHOLD = 8;
        constexpr int STMT_MOUNTAIN_THRESHOLD = 9;

        sqlite3_bind_text(stmt, STMT_MAP_NAME, map_name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, STMT_SEED, config.seed);
        sqlite3_bind_double(stmt, STMT_NOISE_SCALE, static_cast<double>(config.noise_scale));
        sqlite3_bind_int(stmt, STMT_NOISE_OCTAVES, config.noise_octaves);
        sqlite3_bind_int(stmt, STMT_CA_ITERATIONS, config.ca_iterations);
        sqlite3_bind_double(stmt, STMT_WATER_THRESHOLD,
                            static_cast<double>(config.water_threshold));
        sqlite3_bind_double(stmt, STMT_GRASS_THRESHOLD,
                            static_cast<double>(config.grass_threshold));
        sqlite3_bind_double(stmt, STMT_FOREST_THRESHOLD,
                            static_cast<double>(config.forest_threshold));
        sqlite3_bind_double(stmt, STMT_MOUNTAIN_THRESHOLD,
                            static_cast<double>(config.mountain_threshold));

        bool success = false;
        success = sqlite3_step(stmt) == SQLITE_DONE;
        if (!success)
        {
            log_error("Failed to save generator config: " + std::string(sqlite3_errmsg(m_db)));
        }

        sqlite3_finalize(stmt);
        return success;
    }
} // namespace Tactics
