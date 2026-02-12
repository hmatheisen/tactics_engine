#pragma once

#include <cmath>
#include <type_traits>

namespace Tactics
{
    template <typename T>
    class Vector2
    {
        static_assert(std::is_arithmetic_v<T>, "Vector2 requires an arithmetic type");

    public:
        T x;
        T y;

        // Constructors
        constexpr Vector2() : x(0), y(0) {}
        constexpr Vector2(T x_value, T y_value) : x(x_value), y(y_value) {}
        explicit constexpr Vector2(T scalar) : x(scalar), y(scalar) {}

        // Copy constructor
        constexpr Vector2(const Vector2 &other) = default;

        // Assignment operator
        constexpr auto operator=(const Vector2 &other) -> Vector2 & = default;

        // Move constructor
        constexpr Vector2(Vector2 &&other) noexcept = default;

        // Move assignment operator
        constexpr auto operator=(Vector2 &&other) noexcept -> Vector2 & = default;

        // Destructor
        ~Vector2() = default;

        // Arithmetic operators
        [[nodiscard]] constexpr auto operator+(const Vector2 &other) const -> Vector2
        {
            return Vector2(x + other.x, y + other.y);
        }

        [[nodiscard]] constexpr auto operator-(const Vector2 &other) const -> Vector2
        {
            return Vector2(x - other.x, y - other.y);
        }

        [[nodiscard]] constexpr auto operator*(T scalar) const -> Vector2
        {
            return Vector2(x * scalar, y * scalar);
        }

        [[nodiscard]] constexpr auto operator/(T scalar) const -> Vector2
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
        [[nodiscard]] constexpr auto operator-() const -> Vector2
        {
            return Vector2(-x, -y);
        }

        // Comparison operators
        [[nodiscard]] constexpr auto operator==(const Vector2 &other) const -> bool
        {
            return x == other.x && y == other.y;
        }

        [[nodiscard]] constexpr auto operator!=(const Vector2 &other) const -> bool
        {
            return !(*this == other);
        }

        // Vector operations
        [[nodiscard]] auto dot(const Vector2 &other) const -> T
        {
            return (x * other.x) + (y * other.y);
        }

        [[nodiscard]] auto cross(const Vector2 &other) const -> T
        {
            return (x * other.y) - (y * other.x);
        }

        [[nodiscard]] auto length_squared() const -> T
        {
            return (x * x) + (y * y);
        }

        [[nodiscard]] auto length() const -> float
        {
            return std::sqrt(static_cast<float>(length_squared()));
        }

        [[nodiscard]] auto normalize() -> Vector2 &
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

        [[nodiscard]] auto normalized() const -> Vector2
        {
            Vector2 result = *this;
            return result.normalize();
        }

        [[nodiscard]] auto distance_to(const Vector2 &other) const -> float
        {
            return (*this - other).length();
        }

        [[nodiscard]] auto distance_squared_to(const Vector2 &other) const -> T
        {
            return (*this - other).length_squared();
        }

        // Linear interpolation
        [[nodiscard]] static auto lerp(const Vector2 &start, const Vector2 &end, float alpha)
            -> Vector2
        {
            return Vector2(static_cast<T>(start.x + ((end.x - start.x) * alpha)),
                           static_cast<T>(start.y + ((end.y - start.y) * alpha)));
        }

        // Zero vector
        [[nodiscard]] static constexpr auto zero() -> Vector2
        {
            return Vector2(0, 0);
        }

        // Unit vectors
        [[nodiscard]] static constexpr auto up() -> Vector2
        {
            return Vector2(0, 1);
        }

        [[nodiscard]] static constexpr auto down() -> Vector2
        {
            return Vector2(0, -1);
        }

        [[nodiscard]] static constexpr auto left() -> Vector2
        {
            return Vector2(-1, 0);
        }

        [[nodiscard]] static constexpr auto right() -> Vector2
        {
            return Vector2(1, 0);
        }
    };

    // Scalar multiplication from left side
    template <typename T>
    [[nodiscard]] auto operator*(T scalar, const Vector2<T> &vec) -> Vector2<T>
    {
        return vec * scalar;
    }

    // Common type aliases
    using Vector2f = Vector2<float>;
    using Vector2d = Vector2<double>;
    using Vector2i = Vector2<int>;
    using Vector2u = Vector2<unsigned int>;
} // namespace Tactics
