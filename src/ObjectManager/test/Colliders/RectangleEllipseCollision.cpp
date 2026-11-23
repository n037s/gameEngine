#include "EllipseCollider.h"
#include "RectCollider.h"
#include "UnitTestEnvironment.h"

namespace
{
    void expectCollision(UnitTest::Scenario& scenario, const RectCollider& rectangle,
        const EllipseCollider& ellipse, bool expected)
    {
        UT_EXPECT(scenario, collides(rectangle, ellipse) == expected);
        UT_EXPECT(scenario, collides(ellipse, rectangle) == expected);
    }
}

int main()
{
    UnitTest::Environment tests("RectangleEllipseCollision");
    tests.addScenario("Overlap and containment", [](UnitTest::Scenario& scenario) {
        RectCollider rectangle;
        rectangle.setSize(size2D(4, 4));
        EllipseCollider ellipse;
        ellipse.setRadius(size2D(1, 1));

        ellipse.setPosition(point2D(2, 2));
        expectCollision(scenario, rectangle, ellipse, true);
        ellipse.setPosition(point2D(4.5f, 2));
        expectCollision(scenario, rectangle, ellipse, true);
        ellipse.setPosition(point2D(2, 2));
        ellipse.setRadius(size2D(10, 10));
        expectCollision(scenario, rectangle, ellipse, true);
    });
    tests.addScenario("Edges and corners", [](UnitTest::Scenario& scenario) {
        RectCollider rectangle;
        rectangle.setSize(size2D(4, 4));
        EllipseCollider ellipse;
        ellipse.setRadius(size2D(1, 1));

        ellipse.setPosition(point2D(5, 2));
        expectCollision(scenario, rectangle, ellipse, true);
        ellipse.setPosition(point2D(-1, 2));
        expectCollision(scenario, rectangle, ellipse, true);
        ellipse.setPosition(point2D(2, 5));
        expectCollision(scenario, rectangle, ellipse, true);
        ellipse.setPosition(point2D(5.5f, 2));
        expectCollision(scenario, rectangle, ellipse, false);
        ellipse.setRadius(size2D(5, 5));
        ellipse.setPosition(point2D(7, 8));
        expectCollision(scenario, rectangle, ellipse, true);
        ellipse.setPosition(point2D(7, 8.5f));
        expectCollision(scenario, rectangle, ellipse, false);
    });
    tests.addScenario("Unequal radii and translated rectangle", [](UnitTest::Scenario& scenario) {
        RectCollider rectangle;
        rectangle.setPosition(point2D(-4, -4));
        rectangle.setSize(size2D(4, 4));
        EllipseCollider ellipse;
        ellipse.setRadius(size2D(2, 1));

        ellipse.setPosition(point2D(0.5f, 0.75f));
        expectCollision(scenario, rectangle, ellipse, true);
        ellipse.setPosition(point2D(1.5f, 0.75f));
        expectCollision(scenario, rectangle, ellipse, false);
    });
    tests.addScenario("Points and line segments", [](UnitTest::Scenario& scenario) {
        RectCollider rectangle;
        rectangle.setSize(size2D(4, 4));
        EllipseCollider ellipse;

        ellipse.setPosition(point2D(4, 4));
        expectCollision(scenario, rectangle, ellipse, true);
        ellipse.setPosition(point2D(5, 4));
        expectCollision(scenario, rectangle, ellipse, false);
        ellipse.setRadius(size2D(2, 0));
        ellipse.setPosition(point2D(6, 2));
        expectCollision(scenario, rectangle, ellipse, true);
        ellipse.setPosition(point2D(6, 5));
        expectCollision(scenario, rectangle, ellipse, false);
        ellipse.setRadius(size2D(0, 2));
        ellipse.setPosition(point2D(2, 6));
        expectCollision(scenario, rectangle, ellipse, true);
        ellipse.setPosition(point2D(5, 6));
        expectCollision(scenario, rectangle, ellipse, false);
    });
    return tests.run();
}