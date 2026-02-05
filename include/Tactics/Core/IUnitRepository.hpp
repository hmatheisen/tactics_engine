#pragma once

#include "Tactics/Core/Unit.hpp"
#include <string>
#include <vector>

namespace Tactics
{
    class IUnitRepository
    {
    public:
        IUnitRepository() = default;
        virtual ~IUnitRepository() = default;

        IUnitRepository(const IUnitRepository &) = delete;
        auto operator=(const IUnitRepository &) -> IUnitRepository & = delete;

        IUnitRepository(IUnitRepository &&) = delete;
        auto operator=(IUnitRepository &&) -> IUnitRepository & = delete;

        [[nodiscard]] virtual auto load_units(const std::string &map_name) -> std::vector<Unit> = 0;
        virtual auto save_units(const std::string &map_name, const std::vector<Unit> &units)
            -> bool = 0;
    };
} // namespace Tactics
