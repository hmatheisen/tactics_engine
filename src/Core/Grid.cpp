#include "SDL3/SDL_stdinc.h"
#include <Tactics/Core/Grid.hpp>
#include <Tactics/Core/Logger.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <nlohmann/json.hpp>

namespace Tactics
{
    Grid::Grid() : m_width(0), m_height(0) {}

    Grid::Grid(int width, int height) : m_width(width), m_height(height)
    {
        initialize(width, height);
    }

    auto Grid::get_width() const -> int
    {
        return m_width;
    }

    auto Grid::get_height() const -> int
    {
        return m_height;
    }

    auto Grid::get_tile(const Vector2i &position) -> Tile *
    {
        if (!is_valid_position(position))
        {
            log_warning("Attempted to get tile at invalid position: (" +
                        std::to_string(position.x) + ", " + std::to_string(position.y) + ")");
            return nullptr;
        }

        return &m_tiles[index_of(position.x, position.y)];
    }

    auto Grid::get_tile(const Vector2i &position) const -> const Tile *
    {
        if (!is_valid_position(position))
        {
            return nullptr;
        }

        return &m_tiles[index_of(position.x, position.y)];
    }

    void Grid::set_tile(const Vector2i &position, const Tile &tile)
    {
        if (!is_valid_position(position))
        {
            log_warning("Attempted to set tile at invalid position: (" +
                        std::to_string(position.x) + ", " + std::to_string(position.y) + ")");
            return;
        }

        m_tiles[index_of(position.x, position.y)] = tile;
        m_tiles[index_of(position.x, position.y)].set_position(position);
        log_debug("Tile set at position (" + std::to_string(position.x) + ", " +
                  std::to_string(position.y) +
                  "), type: " + std::to_string(static_cast<int>(tile.get_type())) +
                  ", move_cost: " + std::to_string(tile.get_move_cost()));
    }

    void Grid::initialize(int width, int height)
    {
        log_info("Initializing grid: " + std::to_string(width) + "x" + std::to_string(height));
        m_width = width;
        m_height = height;
        m_tiles.clear();
        m_tiles.resize(static_cast<size_t>(width) * static_cast<size_t>(height));

        // Initialize all tiles with default values
        for (int y_pos = 0; y_pos < height; ++y_pos)
        {
            for (int x_pos = 0; x_pos < width; ++x_pos)
            {
                Tile tile(Vector2i(x_pos, y_pos), TileType::Grass, 1);
                m_tiles[index_of(x_pos, y_pos)] = tile;
            }
        }
        log_debug("Grid initialized with " + std::to_string(m_tiles.size()) + " tiles");
    }

    namespace
    {
        auto string_to_tile_type(const std::string &type_str) -> TileType
        {
            if (type_str == "Grass")
            {
                return TileType::Grass;
            }
            if (type_str == "Water")
            {
                return TileType::Water;
            }
            if (type_str == "Mountain")
            {
                return TileType::Mountain;
            }
            if (type_str == "Forest")
            {
                return TileType::Forest;
            }
            if (type_str == "Desert")
            {
                return TileType::Desert;
            }
            if (type_str == "Road")
            {
                return TileType::Road;
            }
            if (type_str == "Wall")
            {
                return TileType::Wall;
            }
            log_warning("Unknown tile type: " + type_str + ", defaulting to Grass");
            return TileType::Grass;
        }
    } // namespace

    auto Grid::load_from_json(const std::string &file_path) -> bool
    {
        log_info("Loading grid from JSON file: " + file_path);

        std::ifstream file(file_path);
        if (!file.is_open())
        {
            log_error("Failed to open JSON file: " + file_path);
            return false;
        }

        nlohmann::json json_data;
        try
        {
            file >> json_data;
        }
        catch (const nlohmann::json::exception &e)
        {
            log_error("Failed to parse JSON file: " + file_path + " - " + std::string(e.what()));
            return false;
        }

        // Validate required fields
        if (!json_data.contains("width") || !json_data.contains("height") ||
            !json_data.contains("tiles"))
        {
            log_error("JSON file missing required fields: width, height, or tiles");
            return false;
        }

        const int width = json_data["width"].get<int>();
        const int height = json_data["height"].get<int>();

        if (width <= 0 || height <= 0)
        {
            log_error("Invalid grid dimensions: " + std::to_string(width) + "x" +
                      std::to_string(height));
            return false;
        }

        // Initialize grid
        m_width = width;
        m_height = height;
        m_tiles.clear();
        m_tiles.resize(static_cast<size_t>(width) * static_cast<size_t>(height));

        // Parse tiles
        if (!json_data["tiles"].is_array())
        {
            log_error("Tiles field must be an array");
            return false;
        }

        const auto &tiles_array = json_data["tiles"];
        int tiles_loaded = 0;

        for (const auto &tile_json : tiles_array)
        {
            if (!tile_json.is_object())
            {
                log_warning("Skipping invalid tile entry (not an object)");
                continue;
            }

            if (!tile_json.contains("x") || !tile_json.contains("y") || !tile_json.contains("type"))
            {
                log_warning("Skipping tile entry missing required fields: x, y, or type");
                continue;
            }

            const int x_pos = tile_json["x"].get<int>();
            const int y_pos = tile_json["y"].get<int>();
            const std::string type_str = tile_json["type"].get<std::string>();
            const int move_cost = tile_json.value("move_cost", 1);

            const Vector2i position(x_pos, y_pos);
            if (!is_valid_position(position))
            {
                log_warning("Skipping tile at invalid position: (" + std::to_string(x_pos) + ", " +
                            std::to_string(y_pos) + ")");
                continue;
            }

            const TileType type = string_to_tile_type(type_str);
            Tile tile(position, type, move_cost);
            set_tile(position, tile);
            ++tiles_loaded;
        }

        log_info("Grid loaded successfully: " + std::to_string(width) + "x" +
                 std::to_string(height) + " with " + std::to_string(tiles_loaded) + " tiles");

        return true;
    }

    auto Grid::save_to_file(const std::string &file_path) const -> bool
    {
        log_info("Saving grid to file: " + file_path);

        std::ofstream file(file_path, std::ios::binary);
        if (!file.is_open())
        {
            log_error("Failed to open file for writing: " + file_path);
            return false;
        }

        try
        {
            boost::archive::binary_oarchive archive(file);
            archive << *this;
            log_info("Grid saved successfully: " + file_path);
            return true;
        }
        catch (const std::exception &e)
        {
            log_error("Failed to serialize grid: " + std::string(e.what()));
            return false;
        }
    }

    auto Grid::load_from_file(const std::string &file_path) -> bool
    {
        log_info("Loading grid from file: " + file_path);

        std::ifstream file(file_path, std::ios::binary);
        if (!file.is_open())
        {
            log_error("Failed to open file for reading: " + file_path);
            return false;
        }

        try
        {
            boost::archive::binary_iarchive archive(file);
            archive >> *this;
            log_info("Grid loaded successfully: " + file_path + " (" + std::to_string(m_width) +
                     "x" + std::to_string(m_height) + " with " + std::to_string(m_tiles.size()) +
                     " tiles)");
            return true;
        }
        catch (const std::exception &e)
        {
            log_error("Failed to deserialize grid: " + std::string(e.what()));
            return false;
        }
    }

    auto Grid::is_valid_position(const Vector2i &position) const -> bool
    {
        return position.x >= 0 && position.x < m_width && position.y >= 0 && position.y < m_height;
    }

    namespace
    {
        // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        auto tile_type_to_color(TileType type) -> SDL_Color
        {
            switch (type)
            {
            case TileType::Grass:
                return {34, 139, 34, 255}; // Green
            case TileType::Water:
                return {0, 119, 190, 255}; // Blue
            case TileType::Mountain:
                return {139, 137, 137, 255}; // Gray
            case TileType::Forest:
                return {34, 100, 34, 255}; // Dark green
            case TileType::Desert:
                return {238, 203, 173, 255}; // Beige
            case TileType::Road:
                return {105, 105, 105, 255}; // Dark gray
            case TileType::Wall:
                return {64, 64, 64, 255}; // Very dark gray
            default:
                return {128, 128, 128, 255}; // Gray
            }
        }
        // NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

    } // namespace

    auto Grid::render(SDL_Renderer *renderer, const Camera &camera) const -> bool
    {
        if (renderer == nullptr)
        {
            return false;
        }

        constexpr float TILE_SIZE = 32.0F; // Size of each tile in world units

        // Get the camera's view rectangle to only render visible tiles
        const Rectf view_rect = camera.get_view_rect();

        // Calculate which tiles are visible
        const int start_x = static_cast<int>((view_rect.left() - TILE_SIZE) / TILE_SIZE);
        const int end_x = static_cast<int>((view_rect.right() + TILE_SIZE) / TILE_SIZE);
        const int start_y = static_cast<int>((view_rect.top() - TILE_SIZE) / TILE_SIZE);
        const int end_y = static_cast<int>((view_rect.bottom() + TILE_SIZE) / TILE_SIZE);

        // Render visible tiles
        for (int y_pos = start_y; y_pos <= end_y; ++y_pos)
        {
            for (int x_pos = start_x; x_pos <= end_x; ++x_pos)
            {
                const Vector2i tile_pos(x_pos, y_pos);
                if (!is_valid_position(tile_pos))
                {
                    continue;
                }

                const Tile *tile = get_tile(tile_pos);
                if (tile == nullptr)
                {
                    continue;
                }

                // Calculate world position of tile
                const float world_x = static_cast<float>(x_pos) * TILE_SIZE;
                const float world_y = static_cast<float>(y_pos) * TILE_SIZE;

                // Convert to screen coordinates
                const Vector2f screen_pos = camera.world_to_screen({world_x, world_y});

                // Calculate screen size (tile size in screen space)
                const float screen_tile_size = TILE_SIZE * camera.get_zoom();

                // Create screen rectangle
                const Rectf screen_rect(screen_pos.x - (screen_tile_size * 0.5F),
                                        screen_pos.y - (screen_tile_size * 0.5F), screen_tile_size,
                                        screen_tile_size);

                // Only render if on screen
                if (screen_rect.right() < 0.0F ||
                    screen_rect.left() > camera.get_viewport_width() ||
                    screen_rect.bottom() < 0.0F || screen_rect.top() > camera.get_viewport_height())
                {
                    continue;
                }

                // Get color for tile type
                const SDL_Color color = tile_type_to_color(tile->get_type());

                // Draw filled rectangle
                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
                SDL_FRect sdl_rect = {screen_rect.x, screen_rect.y, screen_rect.width,
                                      screen_rect.height};
                SDL_RenderFillRect(renderer, &sdl_rect);

                // Draw border
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_MAX_UINT8);
                SDL_RenderRect(renderer, &sdl_rect);
            }
        }

        return true;
    }

    auto Grid::index_of(int x_pos, int y_pos) const -> size_t
    {
        return (static_cast<size_t>(y_pos) * static_cast<size_t>(m_width)) +
               static_cast<size_t>(x_pos);
    }
} // namespace Tactics
