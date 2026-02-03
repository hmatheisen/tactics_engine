#pragma once

#include "Tactics/Core/Grid.hpp"
#include "Tactics/Core/MapGenerator.hpp"
#include <optional>
#include <string>
#include <vector>

namespace Tactics
{
    // Metadata about a saved map
    struct MapMetadata
    {
        int id;
        std::string name;
        int width;
        int height;
        std::string created_at;
        std::string updated_at;
    };

    // Abstract interface for grid persistence
    class IGridRepository
    {
    public:
        IGridRepository() = default;
        virtual ~IGridRepository() = default;

        IGridRepository(const IGridRepository &) = delete;
        auto operator=(const IGridRepository &) -> IGridRepository & = delete;

        IGridRepository(IGridRepository &&) = delete;
        auto operator=(IGridRepository &&) -> IGridRepository & = delete;

        // Load a grid by name
        [[nodiscard]] virtual auto load_map(const std::string &map_name) -> std::optional<Grid> = 0;

        // Save a grid with a name
        virtual auto save_map(const std::string &map_name, const Grid &grid) -> bool = 0;

        // List all available maps
        [[nodiscard]] virtual auto list_maps() -> std::vector<MapMetadata> = 0;

        // Check if a map exists
        [[nodiscard]] virtual auto map_exists(const std::string &map_name) -> bool = 0;

        // Delete a map
        virtual auto delete_map(const std::string &map_name) -> bool = 0;

        // Load generator config by map name
        [[nodiscard]] virtual auto load_generator_config(const std::string &map_name)
            -> std::optional<GeneratorConfig> = 0;

        // Save generator config by map name
        virtual auto save_generator_config(const std::string &map_name,
                                           const GeneratorConfig &config) -> bool = 0;
    };
} // namespace Tactics
