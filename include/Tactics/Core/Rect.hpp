#pragma once

#include "Tactics/Core/Vector2.hpp"
#include <algorithm>
#include <type_traits>

namespace Tactics
{
    template <typename T>
    class Rect
    {
        static_assert(std::is_arithmetic_v<T>, "Rect requires an arithmetic type");

    public:
        T x;
        T y;
        T width;
        T height;

        // Constructors
        Rect() : x(0), y(0), width(0), height(0) {}

        // NOLINTBEGIN(bugprone-easily-swappable-parameters)
        Rect(T x_pos, T y_pos, T width, T height) : x(x_pos), y(y_pos), width(width), height(height)
        {}

        Rect(const Vector2<T> &position, const Vector2<T> &size)
            : x(position.x), y(position.y), width(size.x), height(size.y)
        {}

        Rect(const Vector2<T> &position, T width, T height)
            : x(position.x), y(position.y), width(width), height(height)
        {}
        // NOLINTEND(bugprone-easily-swappable-parameters)

        // Copy constructor
        Rect(const Rect &other) = default;

        // Assignment operator
        auto operator=(const Rect &other) -> Rect & = default;

        // Move constructor
        Rect(Rect &&other) noexcept = default;

        // Move assignment operator
        auto operator=(Rect &&other) noexcept -> Rect & = default;

        // Destructor
        ~Rect() = default;

        // Comparison operators
        auto operator==(const Rect &other) const -> bool
        {
            return x == other.x && y == other.y && width == other.width && height == other.height;
        }

        auto operator!=(const Rect &other) const -> bool
        {
            return !(*this == other);
        }

        // Position and size accessors
        [[nodiscard]] auto position() const -> Vector2<T>
        {
            return Vector2<T>(x, y);
        }

        void set_position(const Vector2<T> &pos)
        {
            x = pos.x;
            y = pos.y;
        }

        void set_position(T x_pos, T y_pos)
        {
            x = x_pos;
            y = y_pos;
        }

        [[nodiscard]] auto size() const -> Vector2<T>
        {
            return Vector2<T>(width, height);
        }

        void set_size(const Vector2<T> &size)
        {
            width = size.x;
            height = size.y;
        }

        // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
        void set_size(T width, T height)
        {
            this->width = width;
            this->height = height;
        }

        // Edge accessors
        [[nodiscard]] auto left() const -> T
        {
            return x;
        }

        [[nodiscard]] auto top() const -> T
        {
            return y;
        }

        [[nodiscard]] auto right() const -> T
        {
            return x + width;
        }

        [[nodiscard]] auto bottom() const -> T
        {
            return y + height;
        }

        // Corner accessors
        [[nodiscard]] auto top_left() const -> Vector2<T>
        {
            return Vector2<T>(x, y);
        }

        [[nodiscard]] auto top_right() const -> Vector2<T>
        {
            return Vector2<T>(x + width, y);
        }

        [[nodiscard]] auto bottom_left() const -> Vector2<T>
        {
            return Vector2<T>(x, y + height);
        }

        [[nodiscard]] auto bottom_right() const -> Vector2<T>
        {
            return Vector2<T>(x + width, y + height);
        }

        // Center accessor
        [[nodiscard]] auto center() const -> Vector2<float>
        {
            constexpr float half = 0.5F;

            return {static_cast<float>(x) + (static_cast<float>(width) * half),
                    static_cast<float>(y) + (static_cast<float>(height) * half)};
        }

        // Area
        [[nodiscard]] auto area() const -> T
        {
            return width * height;
        }

        // Check if rectangle is empty (zero or negative size)
        [[nodiscard]] auto is_empty() const -> bool
        {
            return width <= 0 || height <= 0;
        }

        // Check if a point is inside the rectangle
        [[nodiscard]] auto contains(T point_x, T point_y) const -> bool
        {
            return point_x >= x && point_x < x + width && point_y >= y && point_y < y + height;
        }

        [[nodiscard]] auto contains(const Vector2<T> &point) const -> bool
        {
            return contains(point.x, point.y);
        }

        // Check if another rectangle is completely inside this rectangle
        [[nodiscard]] auto contains(const Rect &other) const -> bool
        {
            return other.x >= x && other.y >= y && other.right() <= right() &&
                   other.bottom() <= bottom();
        }

        // Check if this rectangle intersects with another rectangle
        [[nodiscard]] auto intersects(const Rect &other) const -> bool
        {
            return !(other.x >= x + width || other.x + other.width <= x || other.y >= y + height ||
                     other.y + other.height <= y);
        }

        // Get the intersection rectangle (overlapping area)
        [[nodiscard]] auto intersection(const Rect &other) const -> Rect
        {
            T left_edge = std::max(x, other.x);
            T top_edge = std::max(y, other.y);
            T right_edge = std::min(right(), other.right());
            T bottom_edge = std::min(bottom(), other.bottom());

            if (left_edge >= right_edge || top_edge >= bottom_edge)
            {
                return Rect(0, 0, 0, 0); // Empty rectangle
            }

            return Rect(left_edge, top_edge, right_edge - left_edge, bottom_edge - top_edge);
        }

        // Get the union rectangle (bounding box of both rectangles)
        [[nodiscard]] auto union_rect(const Rect &other) const -> Rect
        {
            if (is_empty())
            {
                return other;
            }
            if (other.is_empty())
            {
                return *this;
            }

            T left_edge = std::min(x, other.x);
            T top_edge = std::min(y, other.y);
            T right_edge = std::max(right(), other.right());
            T bottom_edge = std::max(bottom(), other.bottom());

            return Rect(left_edge, top_edge, right_edge - left_edge, bottom_edge - top_edge);
        }

        // Expand the rectangle by a given amount
        void expand(T amount)
        {
            x -= amount;
            y -= amount;
            width += amount * 2;
            height += amount * 2;
        }

        void expand(T expand_x, T expand_y)
        {
            x -= expand_x;
            y -= expand_y;
            width += expand_x * 2;
            height += expand_y * 2;
        }

        void expand(const Vector2<T> &amount)
        {
            expand(amount.x, amount.y);
        }

        // Shrink the rectangle by a given amount
        void shrink(T amount)
        {
            expand(-amount);
        }

        void shrink(T shrink_x, T shrink_y)
        {
            expand(-shrink_x, -shrink_y);
        }

        void shrink(const Vector2<T> &amount)
        {
            shrink(amount.x, amount.y);
        }

        // Translate (move) the rectangle
        void translate(T offset_x, T offset_y)
        {
            x += offset_x;
            y += offset_y;
        }

        void translate(const Vector2<T> &offset)
        {
            translate(offset.x, offset.y);
        }

        // Get translated rectangle (returns new rectangle)
        [[nodiscard]] auto translated(T offset_x, T offset_y) const -> Rect
        {
            Rect result = *this;
            result.translate(offset_x, offset_y);
            return result;
        }

        [[nodiscard]] auto translated(const Vector2<T> &offset) const -> Rect
        {
            return translated(offset.x, offset.y);
        }

        // Inflate (grow from center)
        void inflate(T amount)
        {
            x -= amount;
            y -= amount;
            width += amount * 2;
            height += amount * 2;
        }

        void inflate(T inflate_x, T inflate_y)
        {
            x -= inflate_x;
            y -= inflate_y;
            width += inflate_x * 2;
            height += inflate_y * 2;
        }

        // Deflate (shrink from center)
        void deflate(T amount)
        {
            inflate(-amount);
        }

        void deflate(T deflate_x, T deflate_y)
        {
            inflate(-deflate_x, -deflate_y);
        }

        // Clamp a point to be within the rectangle bounds
        [[nodiscard]] auto clamp_point(const Vector2<T> &point) const -> Vector2<T>
        {
            T clamped_x = std::clamp(point.x, x, x + width);
            T clamped_y = std::clamp(point.y, y, y + height);

            return Vector2<T>(clamped_x, clamped_y);
        }

        // Get the distance from a point to the rectangle
        [[nodiscard]] auto distance_to_point(const Vector2<T> &point) const -> float
        {
            Vector2<T> clamped = clamp_point(point);

            return clamped.distance_to(point);
        }

        // Static factory methods
        static auto from_edges(T left_edge, T top_edge, T right_edge, T bottom_edge) -> Rect
        {
            return Rect(left_edge, top_edge, right_edge - left_edge, bottom_edge - top_edge);
        }

        static auto from_center(const Vector2<T> &center_pos, T width, T height) -> Rect
        {
            constexpr float half = 0.5F;

            return Rect(static_cast<T>(center_pos.x - width * half),
                        static_cast<T>(center_pos.y - height * half), width, height);
        }

        static auto from_center(const Vector2<T> &center_pos, const Vector2<T> &size) -> Rect
        {
            return from_center(center_pos, size.x, size.y);
        }

        static auto zero() -> Rect
        {
            return Rect(0, 0, 0, 0);
        }
    };

    // Common type aliases
    using Rectf = Rect<float>;
    using Rectd = Rect<double>;
    using Recti = Rect<int>;
    using Rectu = Rect<unsigned int>;
} // namespace Tactics
