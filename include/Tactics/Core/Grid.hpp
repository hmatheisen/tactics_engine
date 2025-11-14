#pragma once

#include <SDL3/SDL.h>
#include <Tactics/Core/Tile.hpp>
#include <vector>

namespace Tactics
{
    class Grid
    {
    public:
        Grid();
        Grid(int width, int height);

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

        // Initialize grid with default tiles
        void initialize(int width, int height);

        // Check if coordinates are valid
        [[nodiscard]] auto is_valid_position(const Vector2i &position) const -> bool;

        // Render the grid
        auto render(SDL_Renderer *renderer) const -> bool;

    private:
        int m_width;
        int m_height;
        std::vector<Tile> m_tiles;

        // Convert 2D coordinates to 1D index
        [[nodiscard]] auto index_of(int x_pos, int y_pos) const -> size_t;
    };
} // namespace Tactics
