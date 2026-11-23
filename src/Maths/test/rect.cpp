#include "rect.h"
#include "UnitTestEnvironment.h"

void verifyCollisionOnX(UnitTest::Scenario& scenario)
{
    const rect first(point2D(0, 0), size2D(4, 4));

    UT_EXPECT(scenario, first.isCollide(rect(point2D(3, 0), size2D(4, 4))));
    UT_EXPECT(scenario, first.isCollide(rect(point2D(-3, 0), size2D(4, 4))));
    UT_EXPECT(scenario, !first.isCollide(rect(point2D(5, 0), size2D(4, 4))));
    UT_EXPECT(scenario, !first.isCollide(rect(point2D(-5, 0), size2D(4, 4))));
}

void verifyCollisionOnY(UnitTest::Scenario& scenario)
{
    const rect first(point2D(0, 0), size2D(4, 4));

    UT_EXPECT(scenario, first.isCollide(rect(point2D(0, 3), size2D(4, 4))));
    UT_EXPECT(scenario, first.isCollide(rect(point2D(0, -3), size2D(4, 4))));
    UT_EXPECT(scenario, !first.isCollide(rect(point2D(0, 5), size2D(4, 4))));
    UT_EXPECT(scenario, !first.isCollide(rect(point2D(0, -5), size2D(4, 4))));
}

void verifyCollisionOnDepth(UnitTest::Scenario& scenario)
{
    const rect first(point2D(0, 0), size2D(4, 4), 0);

    UT_EXPECT(scenario, first.isCollide(rect(point2D(0, 0), size2D(4, 4), 3)));
    UT_EXPECT(scenario, first.isCollide(rect(point2D(0, 0), size2D(4, 4), -3)));
    UT_EXPECT(scenario, !first.isCollide(rect(point2D(0, 0), size2D(4, 4), 5)));
    UT_EXPECT(scenario, !first.isCollide(rect(point2D(0, 0), size2D(4, 4), -5)));
}

void verifyTouchingBoundaries(UnitTest::Scenario& scenario)
{
    const rect first(point2D(0, 0), size2D(4, 4), 0);

    UT_EXPECT(scenario, !first.isCollide(rect(point2D(4, 0), size2D(4, 4))));
    UT_EXPECT(scenario, !first.isCollide(rect(point2D(-4, 0), size2D(4, 4))));
    UT_EXPECT(scenario, !first.isCollide(rect(point2D(0, 4), size2D(4, 4))));
    UT_EXPECT(scenario, !first.isCollide(rect(point2D(0, -4), size2D(4, 4))));
    UT_EXPECT(scenario, !first.isCollide(rect(point2D(0, 0), size2D(4, 4), 4)));
    UT_EXPECT(scenario, !first.isCollide(rect(point2D(0, 0), size2D(4, 4), -4)));
    UT_EXPECT(scenario, !first.isCollide(rect(point2D(4, 4), size2D(4, 4))));
}

void verifyContainmentAndDifferentSizes(UnitTest::Scenario& scenario)
{
    const rect outer(point2D(-2, -2), size2D(8, 8), -2);
    const rect inner(point2D(0, 0), size2D(2, 2), 0);
    const rect same(point2D(-2, -2), size2D(8, 8), -2);

    UT_EXPECT(scenario, outer.isCollide(inner));
    UT_EXPECT(scenario, inner.isCollide(outer));
    UT_EXPECT(scenario, outer.isCollide(same));
    UT_EXPECT(scenario, outer.isCollide(rect(point2D(5.5f, 0), size2D(1, 2), 0)));
    UT_EXPECT(scenario, !outer.isCollide(rect(point2D(6.5f, 0), size2D(1, 2), 0)));
}

void verifyDiagonalCollision(UnitTest::Scenario& scenario)
{
    const rect first(point2D(0, 0), size2D(4, 4));
    const rect overlapping(point2D(3, 3), size2D(2, 2));
    const rect separatedOnX(point2D(5, 3), size2D(2, 2));
    const rect separatedOnY(point2D(3, 5), size2D(2, 2));

    UT_EXPECT(scenario, first.isCollide(overlapping));
    UT_EXPECT(scenario, overlapping.isCollide(first));
    UT_EXPECT(scenario, !first.isCollide(separatedOnX));
    UT_EXPECT(scenario, !separatedOnX.isCollide(first));
    UT_EXPECT(scenario, !first.isCollide(separatedOnY));
    UT_EXPECT(scenario, !separatedOnY.isCollide(first));
}

int main()
{
    UnitTest::Environment tests("rect");

    tests.addScenario("Verify rectangle collision on X axis", verifyCollisionOnX);
    tests.addScenario("Verify rectangle collision on Y axis", verifyCollisionOnY);
    tests.addScenario("Verify rectangle collision on depth", verifyCollisionOnDepth);
    tests.addScenario("Verify touching boundaries do not collide", verifyTouchingBoundaries);
    tests.addScenario("Verify containment and different sizes", verifyContainmentAndDifferentSizes);
    tests.addScenario("Verify diagonal collision and symmetry", verifyDiagonalCollision);

    return tests.run();
}