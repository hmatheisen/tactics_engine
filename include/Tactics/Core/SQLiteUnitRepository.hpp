#pragma once

#include "Tactics/Core/IUnitRepository.hpp"
#include <sqlite3.h>

namespace Tactics
{
    class SQLiteUnitRepository : public IUnitRepository
    {
    public:
        explicit SQLiteUnitRepository(const std::string &db_path);
        ~SQLiteUnitRepository() override;

        SQLiteUnitRepository(const SQLiteUnitRepository &) = delete;
        auto operator=(const SQLiteUnitRepository &) -> SQLiteUnitRepository & = delete;

        SQLiteUnitRepository(SQLiteUnitRepository &&other) noexcept;
        auto operator=(SQLiteUnitRepository &&other) noexcept -> SQLiteUnitRepository &;

        [[nodiscard]] auto load_units(const std::string &map_name) -> std::vector<Unit> override;
        auto save_units(const std::string &map_name, const std::vector<Unit> &units)
            -> bool override;

    private:
        sqlite3 *m_db = nullptr;

        auto initialize_schema() -> bool;
        auto execute_statement(const std::string &sql) -> bool;
    };
} // namespace Tactics
