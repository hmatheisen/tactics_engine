#pragma once

#include "Tactics/Core/IGridRepository.hpp"
#include <sqlite3.h>

namespace Tactics
{
    // SQLite-backed implementation of IGridRepository
    class SQLiteGridRepository : public IGridRepository
    {
    public:
        // Constructor: opens or creates the database file
        explicit SQLiteGridRepository(const std::string &db_path);

        // Destructor: closes the database connection
        ~SQLiteGridRepository() override;

        // Delete copy constructor and assignment operator
        SQLiteGridRepository(const SQLiteGridRepository &) = delete;
        auto operator=(const SQLiteGridRepository &) -> SQLiteGridRepository & = delete;

        // Move constructor
        SQLiteGridRepository(SQLiteGridRepository &&other) noexcept;

        // Move assignment operator
        auto operator=(SQLiteGridRepository &&other) noexcept -> SQLiteGridRepository &;

        // IGridRepository interface
        [[nodiscard]] auto load_map(const std::string &map_name) -> std::optional<Grid> override;
        auto save_map(const std::string &map_name, const Grid &grid) -> bool override;
        [[nodiscard]] auto list_maps() -> std::vector<MapMetadata> override;
        [[nodiscard]] auto map_exists(const std::string &map_name) -> bool override;
        auto delete_map(const std::string &map_name) -> bool override;
        [[nodiscard]] auto load_generator_config(const std::string &map_name)
            -> std::optional<GeneratorConfig> override;
        auto save_generator_config(const std::string &map_name,
                                   const GeneratorConfig &config) -> bool override;

    private:
        sqlite3 *m_db = nullptr;

        // Initialize database schema (creates tables if they don't exist)
        auto initialize_schema() -> bool;

        // Helper: execute a SQL statement
        auto execute_statement(const std::string &sql) -> bool;

        // Helper: get map ID by name
        [[nodiscard]] auto get_map_id(const std::string &map_name) -> std::optional<int>;

        // Helper: create or update map metadata
        auto upsert_map_metadata(const std::string &map_name, Vector2i size)
            -> std::optional<int>;
    };
} // namespace Tactics
