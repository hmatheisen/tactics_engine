#pragma once

#include <cmath>
#include <type_traits>

namespace Tactics
{
    template <typename T> class Vector2
    {
        static_assert(std::is_arithmetic_v<T>, "Vector2 requires an arithmetic type");

    public:
        T x;
        T y;

        // Constructors
        Vector2() : x(0), y(0) {}
        Vector2(T x_value, T y_value) : x(x_value), y(y_value) {}
        explicit Vector2(T scalar) : x(scalar), y(scalar) {}

        // Copy constructor
        Vector2(const Vector2 &other) = default;

        // Assignment operator
        auto operator=(const Vector2 &other) -> Vector2 & = default;

        // Move constructor
        Vector2(Vector2 &&other) noexcept = default;

        // Move assignment operator
        auto operator=(Vector2 &&other) noexcept -> Vector2 & = default;

        // Destructor
        ~Vector2() = default;

        // Arithmetic operators
        auto operator+(const Vector2 &other) const -> Vector2
        {
            return Vector2(x + other.x, y + other.y);
        }

        auto operator-(const Vector2 &other) const -> Vector2
        {
            return Vector2(x - other.x, y - other.y);
        }

        auto operator*(T scalar) const -> Vector2
        {
            return Vector2(x * scalar, y * scalar);
        }

        auto operator/(T scalar) const -> Vector2
        {
            return Vector2(x / scalar, y / scalar);
        }

        // Compound assignment operators
        auto operator+=(const Vector2 &other) -> Vector2 &
        {
            x += other.x;
            y += other.y;
            return *this;
        }

        auto operator-=(const Vector2 &other) -> Vector2 &
        {
            x -= other.x;
            y -= other.y;
            return *this;
        }

        auto operator*=(T scalar) -> Vector2 &
        {
            x *= scalar;
            y *= scalar;
            return *this;
        }

        auto operator/=(T scalar) -> Vector2 &
        {
            x /= scalar;
            y /= scalar;
            return *this;
        }

        // Unary operators
        auto operator-() const -> Vector2
        {
            return Vector2(-x, -y);
        }

        // Comparison operators
        auto operator==(const Vector2 &other) const -> bool
        {
            return x == other.x && y == other.y;
        }

        auto operator!=(const Vector2 &other) const -> bool
        {
            return !(*this == other);
        }

        // Vector operations
        auto dot(const Vector2 &other) const -> T
        {
            return (x * other.x) + (y * other.y);
        }

        auto cross(const Vector2 &other) const -> T
        {
            return (x * other.y) - (y * other.x);
        }

        auto length_squared() const -> T
        {
            return (x * x) + (y * y);
        }

        auto length() const -> float
        {
            return std::sqrt(static_cast<float>(length_squared()));
        }

        auto normalize() -> Vector2 &
        {
            float len = length();

            if (len > 0.0F)
            {
                float inv_len = 1.0F / len;
                x = static_cast<T>(x * inv_len);
                y = static_cast<T>(y * inv_len);
            }

            return *this;
        }

        auto normalized() const -> Vector2
        {
            Vector2 result = *this;
            result.normalize();
            return result;
        }

        auto distance_to(const Vector2 &other) const -> float
        {
            return (*this - other).length();
        }

        auto distance_squared_to(const Vector2 &other) const -> T
        {
            return (*this - other).length_squared();
        }

        // Linear interpolation
        static auto lerp(const Vector2 &start, const Vector2 &end, float alpha) -> Vector2
        {
            return Vector2(static_cast<T>(start.x + ((end.x - start.x) * alpha)),
                           static_cast<T>(start.y + ((end.y - start.y) * alpha)));
        }

        // Zero vector
        static auto zero() -> Vector2
        {
            return Vector2(0, 0);
        }

        // Unit vectors
        static auto up() -> Vector2
        {
            return Vector2(0, 1);
        }

        static auto down() -> Vector2
        {
            return Vector2(0, -1);
        }

        static auto left() -> Vector2
        {
            return Vector2(-1, 0);
        }

        static auto right() -> Vector2
        {
            return Vector2(1, 0);
        }
    };

    // Scalar multiplication from left side
    template <typename T> auto operator*(T scalar, const Vector2<T> &vec) -> Vector2<T>
    {
        return vec * scalar;
    }

    // Common type aliases
    using Vector2f = Vector2<float>;
    using Vector2d = Vector2<double>;
    using Vector2i = Vector2<int>;
    using Vector2u = Vector2<unsigned int>;
} // namespace Tactics
