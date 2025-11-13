#include <Tactics/Core/Rect.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

// NOLINTBEGIN
using namespace Tactics;
using Catch::Matchers::WithinRel;

TEST_CASE("Rect Construction", "[Rect]")
{
    SECTION("Default constructor initializes to zero")
    {
        Rectf rect;
        REQUIRE(rect.x == 0.0f);
        REQUIRE(rect.y == 0.0f);
        REQUIRE(rect.width == 0.0f);
        REQUIRE(rect.height == 0.0f);
    }

    SECTION("Component constructor")
    {
        Rectf rect(10.0f, 20.0f, 100.0f, 200.0f);
        REQUIRE(rect.x == 10.0f);
        REQUIRE(rect.y == 20.0f);
        REQUIRE(rect.width == 100.0f);
        REQUIRE(rect.height == 200.0f);
    }

    SECTION("Position and size constructor with Vector2")
    {
        Vector2f pos(10.0f, 20.0f);
        Vector2f size(100.0f, 200.0f);
        Rectf rect(pos, size);
        REQUIRE(rect.x == 10.0f);
        REQUIRE(rect.y == 20.0f);
        REQUIRE(rect.width == 100.0f);
        REQUIRE(rect.height == 200.0f);
    }

    SECTION("Position Vector2 and scalar size constructor")
    {
        Vector2f pos(10.0f, 20.0f);
        Rectf rect(pos, 100.0f, 200.0f);
        REQUIRE(rect.x == 10.0f);
        REQUIRE(rect.y == 20.0f);
        REQUIRE(rect.width == 100.0f);
        REQUIRE(rect.height == 200.0f);
    }

    SECTION("Copy constructor")
    {
        Rectf original(10.0f, 20.0f, 100.0f, 200.0f);
        Rectf copy(original);
        REQUIRE(copy.x == 10.0f);
        REQUIRE(copy.y == 20.0f);
        REQUIRE(copy.width == 100.0f);
        REQUIRE(copy.height == 200.0f);
    }

    SECTION("Different types")
    {
        Recti rect_i(10, 20, 100, 200);
        REQUIRE(rect_i.x == 10);
        REQUIRE(rect_i.y == 20);
        REQUIRE(rect_i.width == 100);
        REQUIRE(rect_i.height == 200);

        Rectd rect_d(1.5, 2.5, 10.5, 20.5);
        REQUIRE(rect_d.x == 1.5);
        REQUIRE(rect_d.y == 2.5);
        REQUIRE(rect_d.width == 10.5);
        REQUIRE(rect_d.height == 20.5);
    }
}

TEST_CASE("Rect Comparison Operators", "[Rect]")
{
    SECTION("Equality")
    {
        Rectf a(10.0f, 20.0f, 100.0f, 200.0f);
        Rectf b(10.0f, 20.0f, 100.0f, 200.0f);
        Rectf c(10.0f, 20.0f, 100.0f, 201.0f);
        REQUIRE(a == b);
        REQUIRE_FALSE(a == c);
    }

    SECTION("Inequality")
    {
        Rectf a(10.0f, 20.0f, 100.0f, 200.0f);
        Rectf b(10.0f, 20.0f, 100.0f, 201.0f);
        REQUIRE(a != b);
        REQUIRE_FALSE(a != a);
    }
}

TEST_CASE("Rect Accessors", "[Rect]")
{
    Rectf rect(10.0f, 20.0f, 100.0f, 200.0f);

    SECTION("Position accessor")
    {
        Vector2f pos = rect.position();
        REQUIRE(pos.x == 10.0f);
        REQUIRE(pos.y == 20.0f);
    }

    SECTION("Set position")
    {
        rect.set_position(30.0f, 40.0f);
        REQUIRE(rect.x == 30.0f);
        REQUIRE(rect.y == 40.0f);
    }

    SECTION("Set position with Vector2")
    {
        rect.set_position(Vector2f(50.0f, 60.0f));
        REQUIRE(rect.x == 50.0f);
        REQUIRE(rect.y == 60.0f);
    }

    SECTION("Size accessor")
    {
        Vector2f size = rect.size();
        REQUIRE(size.x == 100.0f);
        REQUIRE(size.y == 200.0f);
    }

    SECTION("Set size")
    {
        rect.set_size(150.0f, 250.0f);
        REQUIRE(rect.width == 150.0f);
        REQUIRE(rect.height == 250.0f);
    }

    SECTION("Set size with Vector2")
    {
        rect.set_size(Vector2f(150.0f, 250.0f));
        REQUIRE(rect.width == 150.0f);
        REQUIRE(rect.height == 250.0f);
    }

    SECTION("Edge accessors")
    {
        REQUIRE(rect.left() == 10.0f);
        REQUIRE(rect.top() == 20.0f);
        REQUIRE(rect.right() == 110.0f);
        REQUIRE(rect.bottom() == 220.0f);
    }

    SECTION("Corner accessors")
    {
        REQUIRE(rect.top_left() == Vector2f(10.0f, 20.0f));
        REQUIRE(rect.top_right() == Vector2f(110.0f, 20.0f));
        REQUIRE(rect.bottom_left() == Vector2f(10.0f, 220.0f));
        REQUIRE(rect.bottom_right() == Vector2f(110.0f, 220.0f));
    }

    SECTION("Center accessor")
    {
        Vector2f center = rect.center();
        REQUIRE_THAT(center.x, WithinRel(60.0f, 0.001f));  // 10 + 100/2
        REQUIRE_THAT(center.y, WithinRel(120.0f, 0.001f)); // 20 + 200/2
    }

    SECTION("Area")
    {
        REQUIRE(rect.area() == 20000.0f); // 100 * 200
    }
}

TEST_CASE("Rect Empty Check", "[Rect]")
{
    SECTION("Non-empty rectangle")
    {
        Rectf rect(10.0f, 20.0f, 100.0f, 200.0f);
        REQUIRE_FALSE(rect.is_empty());
    }

    SECTION("Zero size rectangle")
    {
        Rectf rect(10.0f, 20.0f, 0.0f, 200.0f);
        REQUIRE(rect.is_empty());
    }

    SECTION("Negative width")
    {
        Rectf rect(10.0f, 20.0f, -10.0f, 200.0f);
        REQUIRE(rect.is_empty());
    }

    SECTION("Negative height")
    {
        Rectf rect(10.0f, 20.0f, 100.0f, -20.0f);
        REQUIRE(rect.is_empty());
    }
}

TEST_CASE("Rect Point Containment", "[Rect]")
{
    Rectf rect(10.0f, 20.0f, 100.0f, 200.0f);

    SECTION("Point inside rectangle")
    {
        REQUIRE(rect.contains(50.0f, 100.0f));
        REQUIRE(rect.contains(Vector2f(50.0f, 100.0f)));
    }

    SECTION("Point at top-left corner")
    {
        REQUIRE(rect.contains(10.0f, 20.0f));
    }

    SECTION("Point at bottom-right corner (exclusive)")
    {
        REQUIRE_FALSE(rect.contains(110.0f, 220.0f));
    }

    SECTION("Point outside rectangle")
    {
        REQUIRE_FALSE(rect.contains(5.0f, 15.0f));
        REQUIRE_FALSE(rect.contains(150.0f, 250.0f));
        REQUIRE_FALSE(rect.contains(Vector2f(5.0f, 15.0f)));
    }

    SECTION("Point on left edge")
    {
        REQUIRE(rect.contains(10.0f, 100.0f));
    }

    SECTION("Point on right edge (exclusive)")
    {
        REQUIRE_FALSE(rect.contains(110.0f, 100.0f));
    }
}

TEST_CASE("Rect Rectangle Containment", "[Rect]")
{
    Rectf outer(10.0f, 20.0f, 100.0f, 200.0f);

    SECTION("Rectangle completely inside")
    {
        Rectf inner(20.0f, 30.0f, 50.0f, 100.0f);
        REQUIRE(outer.contains(inner));
    }

    SECTION("Rectangle at same position and size")
    {
        Rectf same(10.0f, 20.0f, 100.0f, 200.0f);
        REQUIRE(outer.contains(same));
    }

    SECTION("Rectangle partially outside")
    {
        Rectf partial(5.0f, 15.0f, 50.0f, 100.0f);
        REQUIRE_FALSE(outer.contains(partial));
    }

    SECTION("Rectangle completely outside")
    {
        Rectf outside(200.0f, 300.0f, 50.0f, 100.0f);
        REQUIRE_FALSE(outer.contains(outside));
    }

    SECTION("Rectangle extending beyond right edge")
    {
        Rectf extending(50.0f, 30.0f, 70.0f, 100.0f);
        REQUIRE_FALSE(outer.contains(extending));
    }
}

TEST_CASE("Rect Intersection", "[Rect]")
{
    SECTION("Intersecting rectangles")
    {
        Rectf a(10.0f, 20.0f, 100.0f, 200.0f);
        Rectf b(50.0f, 50.0f, 100.0f, 100.0f);
        REQUIRE(a.intersects(b));
        REQUIRE(b.intersects(a));
    }

    SECTION("Non-intersecting rectangles")
    {
        Rectf a(10.0f, 20.0f, 100.0f, 200.0f);
        Rectf b(200.0f, 300.0f, 50.0f, 100.0f);
        REQUIRE_FALSE(a.intersects(b));
    }

    SECTION("Adjacent rectangles (touching edges)")
    {
        Rectf a(10.0f, 20.0f, 100.0f, 200.0f);
        Rectf b(110.0f, 20.0f, 50.0f, 100.0f);
        REQUIRE_FALSE(a.intersects(b)); // Touching but not overlapping
    }

    SECTION("One rectangle completely inside another")
    {
        Rectf outer(10.0f, 20.0f, 100.0f, 200.0f);
        Rectf inner(20.0f, 30.0f, 50.0f, 100.0f);
        REQUIRE(outer.intersects(inner));
    }

    SECTION("Empty rectangle intersection")
    {
        Rectf a(10.0f, 20.0f, 100.0f, 200.0f);
        Rectf empty(0.0f, 0.0f, 0.0f, 0.0f);
        REQUIRE_FALSE(a.intersects(empty));
    }
}

TEST_CASE("Rect Intersection Calculation", "[Rect]")
{
    SECTION("Overlapping rectangles")
    {
        Rectf a(10.0f, 20.0f, 100.0f, 200.0f);
        Rectf b(50.0f, 50.0f, 100.0f, 100.0f);
        Rectf intersection = a.intersection(b);

        REQUIRE(intersection.x == 50.0f);
        REQUIRE(intersection.y == 50.0f);
        REQUIRE(intersection.width == 60.0f);   // 110 - 50
        REQUIRE(intersection.height == 100.0f); // 150 - 50
    }

    SECTION("Non-overlapping rectangles")
    {
        Rectf a(10.0f, 20.0f, 100.0f, 200.0f);
        Rectf b(200.0f, 300.0f, 50.0f, 100.0f);
        Rectf intersection = a.intersection(b);

        REQUIRE(intersection.is_empty());
    }

    SECTION("One rectangle inside another")
    {
        Rectf outer(10.0f, 20.0f, 100.0f, 200.0f);
        Rectf inner(20.0f, 30.0f, 50.0f, 100.0f);
        Rectf intersection = outer.intersection(inner);

        REQUIRE(intersection == inner);
    }
}

TEST_CASE("Rect Union", "[Rect]")
{
    SECTION("Union of two rectangles")
    {
        Rectf a(10.0f, 20.0f, 100.0f, 200.0f);
        Rectf b(50.0f, 50.0f, 100.0f, 100.0f);
        Rectf union_rect = a.union_rect(b);

        REQUIRE(union_rect.x == 10.0f);
        REQUIRE(union_rect.y == 20.0f);
        REQUIRE(union_rect.width == 140.0f);  // 150 - 10
        REQUIRE(union_rect.height == 200.0f); // 220 - 20
    }

    SECTION("Union with empty rectangle")
    {
        Rectf a(10.0f, 20.0f, 100.0f, 200.0f);
        Rectf empty(0.0f, 0.0f, 0.0f, 0.0f);
        Rectf union_rect = a.union_rect(empty);

        REQUIRE(union_rect == a);
    }

    SECTION("Union of non-overlapping rectangles")
    {
        Rectf a(10.0f, 20.0f, 100.0f, 200.0f);
        Rectf b(200.0f, 300.0f, 50.0f, 100.0f);
        Rectf union_rect = a.union_rect(b);

        REQUIRE(union_rect.x == 10.0f);
        REQUIRE(union_rect.y == 20.0f);
        REQUIRE(union_rect.width == 240.0f);  // 250 - 10
        REQUIRE(union_rect.height == 380.0f); // 400 - 20
    }
}

TEST_CASE("Rect Expand and Shrink", "[Rect]")
{
    SECTION("Expand by uniform amount")
    {
        Rectf rect(10.0f, 20.0f, 100.0f, 200.0f);
        rect.expand(5.0f);

        REQUIRE(rect.x == 5.0f);
        REQUIRE(rect.y == 15.0f);
        REQUIRE(rect.width == 110.0f);
        REQUIRE(rect.height == 210.0f);
    }

    SECTION("Expand by different amounts")
    {
        Rectf rect(10.0f, 20.0f, 100.0f, 200.0f);
        rect.expand(5.0f, 10.0f);

        REQUIRE(rect.x == 5.0f);
        REQUIRE(rect.y == 10.0f);
        REQUIRE(rect.width == 110.0f);
        REQUIRE(rect.height == 220.0f);
    }

    SECTION("Expand with Vector2")
    {
        Rectf rect(10.0f, 20.0f, 100.0f, 200.0f);
        rect.expand(Vector2f(5.0f, 10.0f));

        REQUIRE(rect.x == 5.0f);
        REQUIRE(rect.y == 10.0f);
        REQUIRE(rect.width == 110.0f);
        REQUIRE(rect.height == 220.0f);
    }

    SECTION("Shrink by uniform amount")
    {
        Rectf rect(10.0f, 20.0f, 100.0f, 200.0f);
        rect.shrink(5.0f);

        REQUIRE(rect.x == 15.0f);
        REQUIRE(rect.y == 25.0f);
        REQUIRE(rect.width == 90.0f);
        REQUIRE(rect.height == 190.0f);
    }

    SECTION("Shrink by different amounts")
    {
        Rectf rect(10.0f, 20.0f, 100.0f, 200.0f);
        rect.shrink(5.0f, 10.0f);

        REQUIRE(rect.x == 15.0f);
        REQUIRE(rect.y == 30.0f);
        REQUIRE(rect.width == 90.0f);
        REQUIRE(rect.height == 180.0f);
    }
}

TEST_CASE("Rect Translate", "[Rect]")
{
    SECTION("Translate by offset")
    {
        Rectf rect(10.0f, 20.0f, 100.0f, 200.0f);
        rect.translate(5.0f, 10.0f);

        REQUIRE(rect.x == 15.0f);
        REQUIRE(rect.y == 30.0f);
        REQUIRE(rect.width == 100.0f);
        REQUIRE(rect.height == 200.0f);
    }

    SECTION("Translate with Vector2")
    {
        Rectf rect(10.0f, 20.0f, 100.0f, 200.0f);
        rect.translate(Vector2f(5.0f, 10.0f));

        REQUIRE(rect.x == 15.0f);
        REQUIRE(rect.y == 30.0f);
    }

    SECTION("Translated returns new rectangle")
    {
        Rectf rect(10.0f, 20.0f, 100.0f, 200.0f);
        Rectf translated = rect.translated(5.0f, 10.0f);

        REQUIRE(translated.x == 15.0f);
        REQUIRE(translated.y == 30.0f);
        REQUIRE(rect.x == 10.0f); // Original unchanged
        REQUIRE(rect.y == 20.0f);
    }

    SECTION("Translated with Vector2")
    {
        Rectf rect(10.0f, 20.0f, 100.0f, 200.0f);
        Rectf translated = rect.translated(Vector2f(5.0f, 10.0f));

        REQUIRE(translated.x == 15.0f);
        REQUIRE(translated.y == 30.0f);
    }
}

TEST_CASE("Rect Inflate and Deflate", "[Rect]")
{
    SECTION("Inflate by uniform amount")
    {
        Rectf rect(10.0f, 20.0f, 100.0f, 200.0f);
        rect.inflate(5.0f);

        REQUIRE(rect.x == 5.0f);
        REQUIRE(rect.y == 15.0f);
        REQUIRE(rect.width == 110.0f);
        REQUIRE(rect.height == 210.0f);
    }

    SECTION("Inflate by different amounts")
    {
        Rectf rect(10.0f, 20.0f, 100.0f, 200.0f);
        rect.inflate(5.0f, 10.0f);

        REQUIRE(rect.x == 5.0f);
        REQUIRE(rect.y == 10.0f);
        REQUIRE(rect.width == 110.0f);
        REQUIRE(rect.height == 220.0f);
    }

    SECTION("Deflate by uniform amount")
    {
        Rectf rect(10.0f, 20.0f, 100.0f, 200.0f);
        rect.deflate(5.0f);

        REQUIRE(rect.x == 15.0f);
        REQUIRE(rect.y == 25.0f);
        REQUIRE(rect.width == 90.0f);
        REQUIRE(rect.height == 190.0f);
    }

    SECTION("Deflate by different amounts")
    {
        Rectf rect(10.0f, 20.0f, 100.0f, 200.0f);
        rect.deflate(5.0f, 10.0f);

        REQUIRE(rect.x == 15.0f);
        REQUIRE(rect.y == 30.0f);
        REQUIRE(rect.width == 90.0f);
        REQUIRE(rect.height == 180.0f);
    }
}

TEST_CASE("Rect Clamp Point", "[Rect]")
{
    Rectf rect(10.0f, 20.0f, 100.0f, 200.0f);

    SECTION("Point inside rectangle")
    {
        Vector2f point(50.0f, 100.0f);
        Vector2f clamped = rect.clamp_point(point);
        REQUIRE(clamped == point);
    }

    SECTION("Point outside left edge")
    {
        Vector2f point(5.0f, 100.0f);
        Vector2f clamped = rect.clamp_point(point);
        REQUIRE(clamped.x == 10.0f);
        REQUIRE(clamped.y == 100.0f);
    }

    SECTION("Point outside right edge")
    {
        Vector2f point(150.0f, 100.0f);
        Vector2f clamped = rect.clamp_point(point);
        REQUIRE(clamped.x == 110.0f);
        REQUIRE(clamped.y == 100.0f);
    }

    SECTION("Point outside top edge")
    {
        Vector2f point(50.0f, 10.0f);
        Vector2f clamped = rect.clamp_point(point);
        REQUIRE(clamped.x == 50.0f);
        REQUIRE(clamped.y == 20.0f);
    }

    SECTION("Point outside bottom edge")
    {
        Vector2f point(50.0f, 250.0f);
        Vector2f clamped = rect.clamp_point(point);
        REQUIRE(clamped.x == 50.0f);
        REQUIRE(clamped.y == 220.0f);
    }

    SECTION("Point outside all edges")
    {
        Vector2f point(5.0f, 10.0f);
        Vector2f clamped = rect.clamp_point(point);
        REQUIRE(clamped == rect.top_left());
    }
}

TEST_CASE("Rect Distance to Point", "[Rect]")
{
    Rectf rect(10.0f, 20.0f, 100.0f, 200.0f);

    SECTION("Point inside rectangle")
    {
        Vector2f point(50.0f, 100.0f);
        float distance = rect.distance_to_point(point);
        REQUIRE(distance == 0.0f);
    }

    SECTION("Point outside rectangle")
    {
        Vector2f point(5.0f, 15.0f);
        float distance = rect.distance_to_point(point);
        REQUIRE_THAT(distance, WithinRel(7.071f, 0.01f)); // sqrt(5^2 + 5^2)
    }

    SECTION("Point on edge")
    {
        Vector2f point(10.0f, 100.0f);
        float distance = rect.distance_to_point(point);
        REQUIRE(distance == 0.0f);
    }
}

TEST_CASE("Rect Static Factory Methods", "[Rect]")
{
    SECTION("From edges")
    {
        Rectf rect = Rectf::from_edges(10.0f, 20.0f, 110.0f, 220.0f);
        REQUIRE(rect.x == 10.0f);
        REQUIRE(rect.y == 20.0f);
        REQUIRE(rect.width == 100.0f);
        REQUIRE(rect.height == 200.0f);
    }

    SECTION("From center with width and height")
    {
        Vector2f center(60.0f, 120.0f);
        Rectf rect = Rectf::from_center(center, 100.0f, 200.0f);
        REQUIRE_THAT(rect.x, WithinRel(10.0f, 0.001f));
        REQUIRE_THAT(rect.y, WithinRel(20.0f, 0.001f));
        REQUIRE(rect.width == 100.0f);
        REQUIRE(rect.height == 200.0f);
    }

    SECTION("From center with Vector2 size")
    {
        Vector2f center(60.0f, 120.0f);
        Vector2f size(100.0f, 200.0f);
        Rectf rect = Rectf::from_center(center, size);
        REQUIRE_THAT(rect.x, WithinRel(10.0f, 0.001f));
        REQUIRE_THAT(rect.y, WithinRel(20.0f, 0.001f));
        REQUIRE(rect.width == 100.0f);
        REQUIRE(rect.height == 200.0f);
    }

    SECTION("Zero rectangle")
    {
        Rectf rect = Rectf::zero();
        REQUIRE(rect.x == 0.0f);
        REQUIRE(rect.y == 0.0f);
        REQUIRE(rect.width == 0.0f);
        REQUIRE(rect.height == 0.0f);
        REQUIRE(rect.is_empty());
    }
}

TEST_CASE("Rect Type Aliases", "[Rect]")
{
    SECTION("Rectf")
    {
        Rectf rect(1.5f, 2.5f, 10.5f, 20.5f);
        REQUIRE(rect.x == 1.5f);
        REQUIRE(rect.y == 2.5f);
    }

    SECTION("Rectd")
    {
        Rectd rect(1.5, 2.5, 10.5, 20.5);
        REQUIRE(rect.x == 1.5);
        REQUIRE(rect.y == 2.5);
    }

    SECTION("Recti")
    {
        Recti rect(10, 20, 100, 200);
        REQUIRE(rect.x == 10);
        REQUIRE(rect.y == 20);
    }

    SECTION("Rectu")
    {
        Rectu rect(10u, 20u, 100u, 200u);
        REQUIRE(rect.x == 10u);
        REQUIRE(rect.y == 20u);
    }
}

TEST_CASE("Rect Integer Operations", "[Rect]")
{
    SECTION("Integer rectangle operations")
    {
        Recti a(10, 20, 100, 200);
        Recti b(50, 50, 100, 100);
        REQUIRE(a.intersects(b));
    }

    SECTION("Integer rectangle containment")
    {
        Recti outer(10, 20, 100, 200);
        Recti inner(20, 30, 50, 100);
        REQUIRE(outer.contains(inner));
    }

    SECTION("Integer rectangle area")
    {
        Recti rect(10, 20, 100, 200);
        REQUIRE(rect.area() == 20000);
    }
}
// NOLINTEND
