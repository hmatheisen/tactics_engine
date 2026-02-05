#include "Tactics/Core/Vector2.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

// NOLINTBEGIN
using namespace Tactics;
using Catch::Matchers::WithinRel;

TEST_CASE("Vector2 Construction", "[Vector2]")
{
    SECTION("Default constructor initializes to zero")
    {
        Vector2f vec;
        REQUIRE(vec.x == 0.0f);
        REQUIRE(vec.y == 0.0f);
    }

    SECTION("Component constructor")
    {
        Vector2f vec(3.0f, 4.0f);
        REQUIRE(vec.x == 3.0f);
        REQUIRE(vec.y == 4.0f);
    }

    SECTION("Scalar constructor")
    {
        Vector2f vec(5.0f);
        REQUIRE(vec.x == 5.0f);
        REQUIRE(vec.y == 5.0f);
    }

    SECTION("Copy constructor")
    {
        Vector2f original(1.0f, 2.0f);
        Vector2f copy(original);
        REQUIRE(copy.x == 1.0f);
        REQUIRE(copy.y == 2.0f);
    }

    SECTION("Different types")
    {
        Vector2i vec_i(10, 20);
        REQUIRE(vec_i.x == 10);
        REQUIRE(vec_i.y == 20);

        Vector2d vec_d(1.5, 2.5);
        REQUIRE(vec_d.x == 1.5);
        REQUIRE(vec_d.y == 2.5);
    }
}

TEST_CASE("Vector2 Arithmetic Operators", "[Vector2]")
{
    SECTION("Addition")
    {
        Vector2f a(1.0f, 2.0f);
        Vector2f b(3.0f, 4.0f);
        Vector2f result = a + b;
        REQUIRE(result.x == 4.0f);
        REQUIRE(result.y == 6.0f);
    }

    SECTION("Subtraction")
    {
        Vector2f a(5.0f, 7.0f);
        Vector2f b(2.0f, 3.0f);
        Vector2f result = a - b;
        REQUIRE(result.x == 3.0f);
        REQUIRE(result.y == 4.0f);
    }

    SECTION("Scalar multiplication")
    {
        Vector2f vec(2.0f, 3.0f);
        Vector2f result = vec * 2.0f;
        REQUIRE(result.x == 4.0f);
        REQUIRE(result.y == 6.0f);
    }

    SECTION("Left-side scalar multiplication")
    {
        Vector2f vec(2.0f, 3.0f);
        Vector2f result = 2.0f * vec;
        REQUIRE(result.x == 4.0f);
        REQUIRE(result.y == 6.0f);
    }

    SECTION("Scalar division")
    {
        Vector2f vec(8.0f, 12.0f);
        Vector2f result = vec / 2.0f;
        REQUIRE_THAT(result.x, WithinRel(4.0f, 0.001f));
        REQUIRE_THAT(result.y, WithinRel(6.0f, 0.001f));
    }

    SECTION("Unary negation")
    {
        Vector2f vec(3.0f, -4.0f);
        Vector2f result = -vec;
        REQUIRE(result.x == -3.0f);
        REQUIRE(result.y == 4.0f);
    }
}

TEST_CASE("Vector2 Compound Assignment Operators", "[Vector2]")
{
    SECTION("Addition assignment")
    {
        Vector2f vec(1.0f, 2.0f);
        vec += Vector2f(3.0f, 4.0f);
        REQUIRE(vec.x == 4.0f);
        REQUIRE(vec.y == 6.0f);
    }

    SECTION("Subtraction assignment")
    {
        Vector2f vec(5.0f, 7.0f);
        vec -= Vector2f(2.0f, 3.0f);
        REQUIRE(vec.x == 3.0f);
        REQUIRE(vec.y == 4.0f);
    }

    SECTION("Multiplication assignment")
    {
        Vector2f vec(2.0f, 3.0f);
        vec *= 2.0f;
        REQUIRE(vec.x == 4.0f);
        REQUIRE(vec.y == 6.0f);
    }

    SECTION("Division assignment")
    {
        Vector2f vec(8.0f, 12.0f);
        vec /= 2.0f;
        REQUIRE_THAT(vec.x, WithinRel(4.0f, 0.001f));
        REQUIRE_THAT(vec.y, WithinRel(6.0f, 0.001f));
    }
}

TEST_CASE("Vector2 Comparison Operators", "[Vector2]")
{
    SECTION("Equality")
    {
        Vector2f a(1.0f, 2.0f);
        Vector2f b(1.0f, 2.0f);
        Vector2f c(1.0f, 3.0f);
        REQUIRE(a == b);
        REQUIRE_FALSE(a == c);
    }

    SECTION("Inequality")
    {
        Vector2f a(1.0f, 2.0f);
        Vector2f b(1.0f, 3.0f);
        REQUIRE(a != b);
        REQUIRE_FALSE(a != a);
    }
}

TEST_CASE("Vector2 Dot Product", "[Vector2]")
{
    SECTION("Basic dot product")
    {
        Vector2f a(1.0f, 2.0f);
        Vector2f b(3.0f, 4.0f);
        float result = a.dot(b);
        REQUIRE(result == 11.0f); // 1*3 + 2*4 = 3 + 8 = 11
    }

    SECTION("Perpendicular vectors have zero dot product")
    {
        Vector2f a(1.0f, 0.0f);
        Vector2f b(0.0f, 1.0f);
        REQUIRE(a.dot(b) == 0.0f);
    }

    SECTION("Dot product with self equals length squared")
    {
        Vector2f vec(3.0f, 4.0f);
        float dot_self = vec.dot(vec);
        float len_sq = vec.length_squared();
        REQUIRE_THAT(dot_self, WithinRel(len_sq, 0.001f));
    }
}

TEST_CASE("Vector2 Cross Product", "[Vector2]")
{
    SECTION("Basic cross product")
    {
        Vector2f a(1.0f, 2.0f);
        Vector2f b(3.0f, 4.0f);
        float result = a.cross(b);
        REQUIRE(result == -2.0f); // 1*4 - 2*3 = 4 - 6 = -2
    }

    SECTION("Cross product is anti-commutative")
    {
        Vector2f a(1.0f, 2.0f);
        Vector2f b(3.0f, 4.0f);
        REQUIRE(a.cross(b) == -b.cross(a));
    }

    SECTION("Parallel vectors have zero cross product")
    {
        Vector2f a(2.0f, 4.0f);
        Vector2f b(1.0f, 2.0f); // b = 0.5 * a
        REQUIRE_THAT(a.cross(b), WithinRel(0.0f, 0.001f));
    }
}

TEST_CASE("Vector2 Length", "[Vector2]")
{
    SECTION("Length squared")
    {
        Vector2f vec(3.0f, 4.0f);
        float len_sq = vec.length_squared();
        REQUIRE(len_sq == 25.0f); // 3*3 + 4*4 = 9 + 16 = 25
    }

    SECTION("Length")
    {
        Vector2f vec(3.0f, 4.0f);
        float len = vec.length();
        REQUIRE_THAT(len, WithinRel(5.0f, 0.001f)); // sqrt(25) = 5
    }

    SECTION("Zero vector has zero length")
    {
        Vector2f vec(0.0f, 0.0f);
        REQUIRE(vec.length() == 0.0f);
        REQUIRE(vec.length_squared() == 0.0f);
    }

    SECTION("Unit vector has length 1")
    {
        Vector2f vec(1.0f, 0.0f);
        REQUIRE_THAT(vec.length(), WithinRel(1.0f, 0.001f));
    }
}

TEST_CASE("Vector2 Normalization", "[Vector2]")
{
    SECTION("Normalize")
    {
        Vector2f vec(3.0f, 4.0f);
        Vector2f normalized = vec.normalize();
        REQUIRE_THAT(normalized.length(), WithinRel(1.0f, 0.001f));
    }

    SECTION("Normalized returns new vector")
    {
        Vector2f vec(3.0f, 4.0f);
        Vector2f normalized = vec.normalized();
        REQUIRE_THAT(normalized.length(), WithinRel(1.0f, 0.001f));
        REQUIRE(vec.length() != 1.0f); // Original unchanged
    }

    SECTION("Normalized vector points in same direction")
    {
        Vector2f vec(3.0f, 4.0f);
        Vector2f normalized = vec.normalized();
        // Check that normalized vector is proportional to original
        float ratio = normalized.x / vec.x;
        REQUIRE_THAT(normalized.y, WithinRel(vec.y * ratio, 0.001f));
    }

    SECTION("Zero vector normalization")
    {
        Vector2f vec(0.0f, 0.0f);
        Vector2f normalized = vec.normalize();
        // Should remain zero (no division by zero)
        REQUIRE(normalized.x == 0.0f);
        REQUIRE(normalized.y == 0.0f);
    }
}

TEST_CASE("Vector2 Distance", "[Vector2]")
{
    SECTION("Distance to")
    {
        Vector2f a(0.0f, 0.0f);
        Vector2f b(3.0f, 4.0f);
        float dist = a.distance_to(b);
        REQUIRE_THAT(dist, WithinRel(5.0f, 0.001f));
    }

    SECTION("Distance squared to")
    {
        Vector2f a(0.0f, 0.0f);
        Vector2f b(3.0f, 4.0f);
        float dist_sq = a.distance_squared_to(b);
        REQUIRE(dist_sq == 25.0f);
    }

    SECTION("Distance to self is zero")
    {
        Vector2f vec(5.0f, 7.0f);
        REQUIRE(vec.distance_to(vec) == 0.0f);
        REQUIRE(vec.distance_squared_to(vec) == 0.0f);
    }

    SECTION("Distance is symmetric")
    {
        Vector2f a(1.0f, 2.0f);
        Vector2f b(4.0f, 6.0f);
        REQUIRE_THAT(a.distance_to(b), WithinRel(b.distance_to(a), 0.001f));
    }
}

TEST_CASE("Vector2 Linear Interpolation", "[Vector2]")
{
    SECTION("Lerp at t=0 returns start")
    {
        Vector2f start(0.0f, 0.0f);
        Vector2f end(10.0f, 20.0f);
        Vector2f result = Vector2f::lerp(start, end, 0.0f);
        REQUIRE_THAT(result.x, WithinRel(0.0f, 0.001f));
        REQUIRE_THAT(result.y, WithinRel(0.0f, 0.001f));
    }

    SECTION("Lerp at t=1 returns end")
    {
        Vector2f start(0.0f, 0.0f);
        Vector2f end(10.0f, 20.0f);
        Vector2f result = Vector2f::lerp(start, end, 1.0f);
        REQUIRE_THAT(result.x, WithinRel(10.0f, 0.001f));
        REQUIRE_THAT(result.y, WithinRel(20.0f, 0.001f));
    }

    SECTION("Lerp at t=0.5 returns midpoint")
    {
        Vector2f start(0.0f, 0.0f);
        Vector2f end(10.0f, 20.0f);
        Vector2f result = Vector2f::lerp(start, end, 0.5f);
        REQUIRE_THAT(result.x, WithinRel(5.0f, 0.001f));
        REQUIRE_THAT(result.y, WithinRel(10.0f, 0.001f));
    }

    SECTION("Lerp with arbitrary t")
    {
        Vector2f start(0.0f, 0.0f);
        Vector2f end(10.0f, 20.0f);
        Vector2f result = Vector2f::lerp(start, end, 0.3f);
        REQUIRE_THAT(result.x, WithinRel(3.0f, 0.001f));
        REQUIRE_THAT(result.y, WithinRel(6.0f, 0.001f));
    }
}

TEST_CASE("Vector2 Static Factory Methods", "[Vector2]")
{
    SECTION("Zero vector")
    {
        Vector2f vec = Vector2f::zero();
        REQUIRE(vec.x == 0.0f);
        REQUIRE(vec.y == 0.0f);
    }

    SECTION("Unit vectors")
    {
        Vector2f up = Vector2f::up();
        REQUIRE(up.x == 0.0f);
        REQUIRE(up.y == 1.0f);

        Vector2f down = Vector2f::down();
        REQUIRE(down.x == 0.0f);
        REQUIRE(down.y == -1.0f);

        Vector2f left = Vector2f::left();
        REQUIRE(left.x == -1.0f);
        REQUIRE(left.y == 0.0f);

        Vector2f right = Vector2f::right();
        REQUIRE(right.x == 1.0f);
        REQUIRE(right.y == 0.0f);
    }

    SECTION("Unit vectors have length 1")
    {
        REQUIRE_THAT(Vector2f::up().length(), WithinRel(1.0f, 0.001f));
        REQUIRE_THAT(Vector2f::down().length(), WithinRel(1.0f, 0.001f));
        REQUIRE_THAT(Vector2f::left().length(), WithinRel(1.0f, 0.001f));
        REQUIRE_THAT(Vector2f::right().length(), WithinRel(1.0f, 0.001f));
    }
}

TEST_CASE("Vector2 Type Aliases", "[Vector2]")
{
    SECTION("Vector2f")
    {
        Vector2f vec(1.5f, 2.5f);
        REQUIRE(vec.x == 1.5f);
        REQUIRE(vec.y == 2.5f);
    }

    SECTION("Vector2d")
    {
        Vector2d vec(1.5, 2.5);
        REQUIRE(vec.x == 1.5);
        REQUIRE(vec.y == 2.5);
    }

    SECTION("Vector2i")
    {
        Vector2i vec(10, 20);
        REQUIRE(vec.x == 10);
        REQUIRE(vec.y == 20);
    }

    SECTION("Vector2u")
    {
        Vector2u vec(10u, 20u);
        REQUIRE(vec.x == 10u);
        REQUIRE(vec.y == 20u);
    }
}

TEST_CASE("Vector2 Integer Operations", "[Vector2]")
{
    SECTION("Integer vector arithmetic")
    {
        Vector2i a(5, 10);
        Vector2i b(2, 3);
        Vector2i result = a + b;
        REQUIRE(result.x == 7);
        REQUIRE(result.y == 13);
    }

    SECTION("Integer vector length")
    {
        Vector2i vec(3, 4);
        float len = vec.length();
        REQUIRE_THAT(len, WithinRel(5.0f, 0.001f));
    }

    SECTION("Integer vector dot product")
    {
        Vector2i a(2, 3);
        Vector2i b(4, 5);
        int result = a.dot(b);
        REQUIRE(result == 23); // 2*4 + 3*5 = 8 + 15 = 23
    }
}
// NOLINTEND
