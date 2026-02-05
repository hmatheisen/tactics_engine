#include "Tactics/Core/SQLiteUnitRepository.hpp"
#include "Tactics/Core/Logger.hpp"

namespace Tactics
{
    SQLiteUnitRepository::SQLiteUnitRepository(const std::string &db_path)
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
            log_error("Failed to initialize unit repository schema");
            if (m_db != nullptr)
            {
                sqlite3_close(m_db);
                m_db = nullptr;
            }
        }
    }

    SQLiteUnitRepository::~SQLiteUnitRepository()
    {
        if (m_db != nullptr)
        {
            sqlite3_close(m_db);
            m_db = nullptr;
        }
    }

    SQLiteUnitRepository::SQLiteUnitRepository(SQLiteUnitRepository &&other) noexcept
        : m_db(other.m_db)
    {
        other.m_db = nullptr;
    }

    auto SQLiteUnitRepository::operator=(SQLiteUnitRepository &&other) noexcept
        -> SQLiteUnitRepository &
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
    auto SQLiteUnitRepository::initialize_schema() -> bool
    {
        if (m_db == nullptr)
        {
            return false;
        }

        const std::string create_units_sql = R"(
            CREATE TABLE IF NOT EXISTS units (
                map_name TEXT NOT NULL,
                unit_index INTEGER NOT NULL,
                x INTEGER NOT NULL,
                y INTEGER NOT NULL,
                move_points INTEGER NOT NULL,
                PRIMARY KEY (map_name, unit_index)
            )
        )";

        if (!execute_statement(create_units_sql))
        {
            return false;
        }

        const std::string create_units_index_sql =
            "CREATE INDEX IF NOT EXISTS idx_units_map_name ON units(map_name)";
        return execute_statement(create_units_index_sql);
    }

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    auto SQLiteUnitRepository::execute_statement(const std::string &sql) -> bool
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

    auto SQLiteUnitRepository::load_units(const std::string &map_name) -> std::vector<Unit>
    {
        std::vector<Unit> units;
        if (m_db == nullptr)
        {
            return units;
        }

        const std::string sql =
            "SELECT x, y, move_points FROM units WHERE map_name = ? ORDER BY unit_index";
        sqlite3_stmt *stmt = nullptr;
        if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        {
            log_error("Failed to prepare units query: " + std::string(sqlite3_errmsg(m_db)));
            return units;
        }

        sqlite3_bind_text(stmt, 1, map_name.c_str(), -1, SQLITE_STATIC);

        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            const int x_pos = sqlite3_column_int(stmt, 0);
            const int y_pos = sqlite3_column_int(stmt, 1);
            const int move_points = sqlite3_column_int(stmt, 2);
            units.emplace_back(Vector2i{x_pos, y_pos}, move_points);
        }

        sqlite3_finalize(stmt);
        return units;
    }

    auto SQLiteUnitRepository::save_units(const std::string &map_name,
                                          const std::vector<Unit> &units) -> bool
    {
        if (m_db == nullptr)
        {
            log_error("Database connection is null");
            return false;
        }

        if (!execute_statement("BEGIN TRANSACTION"))
        {
            return false;
        }

        const std::string delete_sql = "DELETE FROM units WHERE map_name = ?";
        sqlite3_stmt *delete_stmt = nullptr;
        if (sqlite3_prepare_v2(m_db, delete_sql.c_str(), -1, &delete_stmt, nullptr) != SQLITE_OK)
        {
            log_error("Failed to prepare delete units: " + std::string(sqlite3_errmsg(m_db)));
            execute_statement("ROLLBACK");
            return false;
        }

        sqlite3_bind_text(delete_stmt, 1, map_name.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(delete_stmt) != SQLITE_DONE)
        {
            log_error("Failed to delete units: " + std::string(sqlite3_errmsg(m_db)));
            sqlite3_finalize(delete_stmt);
            execute_statement("ROLLBACK");
            return false;
        }
        sqlite3_finalize(delete_stmt);

        const std::string insert_sql =
            "INSERT INTO units (map_name, unit_index, x, y, move_points) VALUES (?, ?, ?, ?, ?)";
        sqlite3_stmt *insert_stmt = nullptr;
        if (sqlite3_prepare_v2(m_db, insert_sql.c_str(), -1, &insert_stmt, nullptr) != SQLITE_OK)
        {
            log_error("Failed to prepare insert units: " + std::string(sqlite3_errmsg(m_db)));
            execute_statement("ROLLBACK");
            return false;
        }

        constexpr int STMT_MAP_NAME = 1;
        constexpr int STMT_UNIT_INDEX = 2;
        constexpr int STMT_X = 3;
        constexpr int STMT_Y = 4;
        constexpr int STMT_MOVE_POINTS = 5;

        for (size_t index = 0; index < units.size(); ++index)
        {
            const Vector2i position = units[index].get_position();
            sqlite3_reset(insert_stmt);
            sqlite3_bind_text(insert_stmt, STMT_MAP_NAME, map_name.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_int(insert_stmt, STMT_UNIT_INDEX, static_cast<int>(index));
            sqlite3_bind_int(insert_stmt, STMT_X, position.x);
            sqlite3_bind_int(insert_stmt, STMT_Y, position.y);
            sqlite3_bind_int(insert_stmt, STMT_MOVE_POINTS, units[index].get_move_points());

            if (sqlite3_step(insert_stmt) != SQLITE_DONE)
            {
                log_error("Failed to insert unit: " + std::string(sqlite3_errmsg(m_db)));
                sqlite3_finalize(insert_stmt);
                execute_statement("ROLLBACK");
                return false;
            }
        }

        sqlite3_finalize(insert_stmt);

        if (!execute_statement("COMMIT"))
        {
            execute_statement("ROLLBACK");
            return false;
        }

        return true;
    }
} // namespace Tactics
