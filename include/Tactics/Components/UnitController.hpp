#pragma once

#include "Tactics/Components/Camera.hpp"
#include "Tactics/Components/Cursor.hpp"
#include "Tactics/Components/Grid.hpp"
#include "Tactics/Components/Unit.hpp"
#include "Tactics/Core/EventBus.hpp"

#include <SDL3/SDL.h>
#include <optional>
#include <vector>

namespace Tactics
{
    class UnitController : public Publisher
    {
    public:
        UnitController() = default;
        ~UnitController() = default;

        // Delete copy constructor and assignment operator
        UnitController(const UnitController &) = delete;
        auto operator=(const UnitController &) -> UnitController & = delete;

        // Delete move constructor and assignment operator
        UnitController(UnitController &&) = delete;
        auto operator=(UnitController &&) -> UnitController & = delete;

        void update(const Grid &grid, const Cursor &cursor);
        void render(SDL_Renderer *renderer, const Camera &camera, float tile_size,
                    const Grid &grid) const;
        void set_units(const Grid &grid, std::vector<Unit> units);
        [[nodiscard]] auto get_units() const -> const std::vector<Unit> &;
        void on_grid_changed(const Grid &grid);
        void clear_selection();

    private:
        static constexpr int DEFAULT_UNIT_MOVE_POINTS = 5;

        std::vector<Unit> m_units;
        std::optional<size_t> m_selected_unit;
        std::vector<int> m_reachable_move_points;

        [[nodiscard]] auto find_unit_index_at(const Vector2i &position) const
            -> std::optional<size_t>;
        [[nodiscard]] auto is_tile_reachable(const Grid &grid, const Vector2i &position) const
            -> bool;
        void compute_reachable_tiles(const Grid &grid, const Unit &unit);
        [[nodiscard]] auto build_occupied_tiles(const Grid &grid, const Vector2i &start) const
            -> std::vector<bool>;
        void expand_reachable_tiles(const Grid &grid, const Unit &unit,
                                    const std::vector<bool> &occupied);
        void render_reachable_tiles(SDL_Renderer *renderer, const Camera &camera, float tile_size,
                                    const Grid &grid) const;
        void clear_reachable_tiles();
        void clamp_units_to_grid(const Grid &grid);
    };
} // namespace Tactics
