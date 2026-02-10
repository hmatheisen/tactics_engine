#include "Tactics/Components/UnitController.hpp"
#include "Tactics/Components/Tile.hpp"
#include "Tactics/Core/InputManager.hpp"
#include <algorithm>
#include <array>
#include <queue>

namespace Tactics
{
    namespace
    {
        auto index_of(const Vector2i &position, int width) -> size_t
        {
            return (static_cast<size_t>(position.y) * static_cast<size_t>(width)) +
                   static_cast<size_t>(position.x);
        }

        constexpr uint8_t REACHABLE_COLOR_R = 80;
        constexpr uint8_t REACHABLE_COLOR_G = 160;
        constexpr uint8_t REACHABLE_COLOR_B = 255;
        constexpr uint8_t REACHABLE_COLOR_A = 120;
    } // namespace

    void UnitController::reset_for_grid(const Grid &grid, const Vector2i &spawn_position)
    {
        m_units.clear();
        m_units.emplace_back(spawn_position, DEFAULT_UNIT_MOVE_POINTS);
        clamp_units_to_grid(grid);
        clear_reachable_tiles();
        m_selected_unit.reset();
    }

    void UnitController::update(const Grid &grid, const Cursor &cursor)
    {
        auto &input = InputManager::instance();
        if (!input.is_key_just_pressed(SDL_SCANCODE_SPACE))
        {
            return;
        }

        const Vector2i cursor_pos = cursor.get_position();
        if (!m_selected_unit.has_value())
        {
            auto unit_index = find_unit_index_at(cursor_pos);
            if (unit_index.has_value())
            {
                m_selected_unit = unit_index;
                compute_reachable_tiles(grid, m_units[unit_index.value()]);
            }

            return;
        }

        const size_t selected_index = m_selected_unit.value();
        const Vector2i unit_pos = m_units[selected_index].get_position();
        if (cursor_pos == unit_pos)
        {
            clear_reachable_tiles();
            m_selected_unit.reset();
            return;
        }

        if (!is_tile_reachable(grid, cursor_pos))
        {
            return;
        }

        auto target_unit = find_unit_index_at(cursor_pos);
        if (target_unit.has_value() && target_unit.value() != selected_index)
        {
            return;
        }

        m_units[selected_index].set_position(cursor_pos);
        clear_reachable_tiles();
        m_selected_unit.reset();
    }

    void UnitController::render(SDL_Renderer *renderer, const Camera &camera, float tile_size,
                                const Grid &grid) const
    {
        if (renderer == nullptr)
        {
            return;
        }

        render_reachable_tiles(renderer, camera, tile_size, grid);

        for (const auto &unit : m_units)
        {
            unit.render(renderer, camera, tile_size);
        }
    }

    void UnitController::set_units(const Grid &grid, std::vector<Unit> units)
    {
        m_units = std::move(units);
        clamp_units_to_grid(grid);
        clear_selection();
    }

    auto UnitController::get_units() const -> const std::vector<Unit> &
    {
        return m_units;
    }

    void UnitController::on_grid_changed(const Grid &grid)
    {
        clamp_units_to_grid(grid);
        clear_selection();
    }

    void UnitController::clear_selection()
    {
        clear_reachable_tiles();
        m_selected_unit.reset();
    }

    auto UnitController::find_unit_index_at(const Vector2i &position) const -> std::optional<size_t>
    {
        for (size_t index = 0; index < m_units.size(); ++index)
        {
            if (m_units[index].get_position() == position)
            {
                return index;
            }
        }

        return std::nullopt;
    }

    auto UnitController::is_tile_reachable(const Grid &grid, const Vector2i &position) const -> bool
    {
        if (!grid.is_valid_position(position))
        {
            return false;
        }

        if (m_reachable_move_points.empty())
        {
            return false;
        }

        const size_t index = index_of(position, grid.get_width());
        return m_reachable_move_points[index] >= 0;
    }

    void UnitController::compute_reachable_tiles(const Grid &grid, const Unit &unit)
    {
        const int width = grid.get_width();
        const int height = grid.get_height();
        const int total_tiles = width * height;
        if (width <= 0 || height <= 0 || total_tiles <= 0)
        {
            clear_reachable_tiles();
            return;
        }

        m_reachable_move_points.assign(static_cast<size_t>(total_tiles), -1);

        const Vector2i start = unit.get_position();
        if (!grid.is_valid_position(start))
        {
            return;
        }

        const size_t start_index = index_of(start, width);
        m_reachable_move_points[start_index] = unit.get_move_points();
        const std::vector<bool> occupied = build_occupied_tiles(grid, start);
        expand_reachable_tiles(grid, unit, occupied);
    }

    auto UnitController::build_occupied_tiles(const Grid &grid, const Vector2i &start) const
        -> std::vector<bool>
    {
        const int width = grid.get_width();
        const int height = grid.get_height();
        const int total_tiles = width * height;

        std::vector<bool> occupied(static_cast<size_t>(total_tiles), false);
        for (const auto &other_unit : m_units)
        {
            const Vector2i position = other_unit.get_position();
            if (position == start || !grid.is_valid_position(position))
            {
                continue;
            }

            const size_t occupied_index = index_of(position, width);
            occupied[occupied_index] = true;
        }

        return occupied;
    }

    void UnitController::expand_reachable_tiles(const Grid &grid, const Unit &unit,
                                                const std::vector<bool> &occupied)
    {
        const int width = grid.get_width();
        const int height = grid.get_height();
        if (width <= 0 || height <= 0)
        {
            return;
        }

        struct Node
        {
            Vector2i position;
            int remaining;
        };

        std::queue<Node> frontier;
        frontier.push(Node{unit.get_position(), unit.get_move_points()});

        const std::array<Vector2i, 4> directions = {Vector2i{0, -1}, Vector2i{0, 1},
                                                    Vector2i{-1, 0}, Vector2i{1, 0}};

        while (!frontier.empty())
        {
            const Node current = frontier.front();
            frontier.pop();

            for (const auto &direction : directions)
            {
                const Vector2i neighbor = current.position + direction;
                if (!grid.is_valid_position(neighbor))
                {
                    continue;
                }

                const size_t neighbor_index = index_of(neighbor, width);
                if (occupied[neighbor_index])
                {
                    continue;
                }

                const Tile *tile = grid.get_tile(neighbor);
                if (tile == nullptr || !tile->is_walkable())
                {
                    continue;
                }

                const int cost = tile->get_move_cost();
                if (cost > current.remaining)
                {
                    continue;
                }

                const int remaining = current.remaining - cost;
                if (remaining > m_reachable_move_points[neighbor_index])
                {
                    m_reachable_move_points[neighbor_index] = remaining;
                    frontier.push(Node{neighbor, remaining});
                }
            }
        }
    }

    void UnitController::render_reachable_tiles(SDL_Renderer *renderer, const Camera &camera,
                                                float tile_size, const Grid &grid) const
    {
        if (renderer == nullptr || m_reachable_move_points.empty())
        {
            return;
        }

        const int width = grid.get_width();
        const int height = grid.get_height();
        if (width <= 0 || height <= 0)
        {
            return;
        }

        const float screen_tile_size = tile_size * camera.get_zoom();
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        for (int row = 0; row < height; ++row)
        {
            for (int col = 0; col < width; ++col)
            {
                const Vector2i position{col, row};
                const size_t index = index_of(position, width);
                if (m_reachable_move_points[index] < 0)
                {
                    continue;
                }

                const float world_x = static_cast<float>(col) * tile_size;
                const float world_y = static_cast<float>(row) * tile_size;
                const Vector2f screen_pos = camera.world_to_screen({world_x, world_y});

                const float left = screen_pos.x - (screen_tile_size * 0.5F);
                const float top = screen_pos.y - (screen_tile_size * 0.5F);
                const float right = left + screen_tile_size;
                const float bottom = top + screen_tile_size;
                if (right < 0.0F || left > camera.get_viewport_width() || bottom < 0.0F ||
                    top > camera.get_viewport_height())
                {
                    continue;
                }

                SDL_SetRenderDrawColor(renderer, REACHABLE_COLOR_R, REACHABLE_COLOR_G,
                                       REACHABLE_COLOR_B, REACHABLE_COLOR_A);
                SDL_FRect rect = {left, top, screen_tile_size, screen_tile_size};
                SDL_RenderFillRect(renderer, &rect);
            }
        }

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }

    void UnitController::clear_reachable_tiles()
    {
        m_reachable_move_points.clear();
    }

    void UnitController::clamp_units_to_grid(const Grid &grid)
    {
        const Vector2i grid_size(grid.get_width(), grid.get_height());
        for (auto &unit : m_units)
        {
            Vector2i position = unit.get_position();
            position.x = std::clamp(position.x, 0, grid_size.x - 1);
            position.y = std::clamp(position.y, 0, grid_size.y - 1);
            unit.set_position(position);
        }
    }
} // namespace Tactics
