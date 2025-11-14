#pragma once

#include <SDL3/SDL.h>
#include <Tactics/Core/Camera.hpp>
#include <Tactics/Core/Tile.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>
#include <optional>
#include <string>
#include <vector>

namespace Tactics
{
    class Grid
    {
    public:
        Grid();

        // Delete copy constructor and assignment operator
        Grid(const Grid &) = delete;
        auto operator=(const Grid &) -> Grid & = delete;

        // Move constructor
        Grid(Grid &&other) noexcept = default;

        // Move assignment operator
        auto operator=(Grid &&other) noexcept -> Grid & = default;

        // Destructor
        ~Grid() = default;

        // Size accessors
        [[nodiscard]] auto get_width() const -> int;
        [[nodiscard]] auto get_height() const -> int;

        // Get tile at position (returns nullptr if out of bounds)
        [[nodiscard]] auto get_tile(const Vector2i &position) -> Tile *;
        [[nodiscard]] auto get_tile(const Vector2i &position) const -> const Tile *;

        // Set tile at position
        void set_tile(const Vector2i &position, const Tile &tile);

        // Save grid to file using Boost serialization
        [[nodiscard]] auto save_to_file(const std::string &file_path) const -> bool;

        // Static factory: Load grid from file using Boost serialization
        static auto load_from_file(const std::string &file_path) -> std::optional<Grid>;

        // Check if coordinates are valid
        [[nodiscard]] auto is_valid_position(const Vector2i &position) const -> bool;

        // Render the grid
        auto render(SDL_Renderer *renderer, const Camera &camera) const -> bool;

    private:
        int m_width;
        int m_height;
        std::vector<Tile> m_tiles;

        // Convert 2D coordinates to 1D index
        [[nodiscard]] auto index_of(int x_pos, int y_pos) const -> size_t;

        // Boost serialization
        friend class boost::serialization::access;

        template <class Archive>
        void serialize(Archive &archive, [[maybe_unused]] const unsigned int version)
        {
            archive & m_width & m_height & m_tiles;
        }
    };
} // namespace Tactics
