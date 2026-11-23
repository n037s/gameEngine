#include "ellipse.h"
#include "UnitTestEnvironment.h"

void VerifyCircleCollisionOnX(UnitTest::Scenario& scenario)
{
    const ellipse first(point2D(0, 0), size2D(2, 2));

    UT_EXPECT(scenario, first.isCollide(ellipse(point2D(3, 0), size2D(2, 2))));
    UT_EXPECT(scenario, first.isCollide(ellipse(point2D(-3, 0), size2D(2, 2))));

    UT_EXPECT(scenario, !first.isCollide(ellipse(point2D(5, 0), size2D(2, 2))));
    UT_EXPECT(scenario, !first.isCollide(ellipse(point2D(-5, 0), size2D(2, 2))));
}

void verifyCircleCollisionOnY(UnitTest::Scenario& scenario)
{
    const ellipse first(point2D(0, 0), size2D(2, 2));
    
    UT_EXPECT(scenario, first.isCollide(ellipse(point2D(0, 3), size2D(2, 2))));
    UT_EXPECT(scenario, first.isCollide(ellipse(point2D(0, -3), size2D(2, 2))));

    UT_EXPECT(scenario, !first.isCollide(ellipse(point2D(0, 5), size2D(2, 2))));
    UT_EXPECT(scenario, !first.isCollide(ellipse(point2D(0, -5), size2D(2, 2))));
}

void verifyCircleRadiiCollision(UnitTest::Scenario& scenario)
{
    const ellipse first(point2D(0, 0), size2D(2, 2));

    // verify with (1, 1) radii
    UT_EXPECT(scenario, first.isCollide(ellipse(point2D(3, 0), size2D(1, 1))));
    UT_EXPECT(scenario, first.isCollide(ellipse(point2D(-3, 0), size2D(1, 1))));

    UT_EXPECT(scenario, !first.isCollide(ellipse(point2D(4, 0), size2D(1, 1))));
    UT_EXPECT(scenario, !first.isCollide(ellipse(point2D(-4, 0), size2D(1, 1))));

    // Verify with larger radii
    UT_EXPECT(scenario, first.isCollide(ellipse(point2D(5, 0), size2D(4, 4))));

    // Verify with floating point radii
    UT_EXPECT(scenario, first.isCollide(ellipse(point2D(1, 0), size2D(0.5, 0.5))));
}

void verifyCircleEdgeCollisionDetection(UnitTest::Scenario& scenario)
{
    const ellipse first(point2D(0, 0), size2D(2, 2));

    UT_EXPECT(scenario, first.isCollide(ellipse(point2D(4, 0), size2D(2, 2))));
    UT_EXPECT(scenario, first.isCollide(ellipse(point2D(-4, 0), size2D(2, 2))));
}

void verifyPointCollision(UnitTest::Scenario& scenario)
{
    const ellipse first(point2D(0, 0), size2D(2, 2));

    UT_EXPECT(scenario, first.isCollide(ellipse(point2D(1, 0), size2D(0, 0))));
    UT_EXPECT(scenario, first.isCollide(ellipse(point2D(-1, 0), size2D(0, 0))));

    UT_EXPECT(scenario, first.isCollide(ellipse(point2D(2, 0), size2D(0, 0))));
    UT_EXPECT(scenario, first.isCollide(ellipse(point2D(-2, 0), size2D(0, 0))));

    UT_EXPECT(scenario, !first.isCollide(ellipse(point2D(3, 0), size2D(0, 0))));
    UT_EXPECT(scenario, !first.isCollide(ellipse(point2D(-3, 0), size2D(0, 0))));
}

void verifyVaryingRadiiCollisionDetection(UnitTest::Scenario& scenario)
{
    const ellipse first(point2D(0, 0), size2D(2, 4));

    // On X Axis, center at 0 and radius at 2
    UT_EXPECT(scenario, first.isCollide(ellipse(point2D(3, 0), size2D(2, 2))));
    UT_EXPECT(scenario, !first.isCollide(ellipse(point2D(5, 0), size2D(2, 2))));

    // On Y Axis, center at 0 and radius at 4
    UT_EXPECT(scenario, first.isCollide(ellipse(point2D(0, 5), size2D(2, 2))));
    UT_EXPECT(scenario, !first.isCollide(ellipse(point2D(0, 7), size2D(2, 2))));

    // A circle on (5, 0) with 2 radii 
    UT_EXPECT(scenario, !first.isCollide(ellipse(point2D(5, 0), size2D(2, 2))));
    UT_EXPECT(scenario, first.isCollide(ellipse(point2D(5, 0), size2D(3, 2))));
}

void verifyVaryingAxisCollisionDetection(UnitTest::Scenario& scenario)
{
    const ellipse first(point2D(0, 0), size2D(2, 2));

    // on the diagonal, the first circle edge is around (1.4, 1.4)
    UT_EXPECT(scenario, !first.isCollide(ellipse(point2D(2, 2), size2D(0.5, 0.5))));
    UT_EXPECT(scenario, first.isCollide(ellipse(point2D(2, 2), size2D(1, 1))));

    UT_EXPECT(scenario, !first.isCollide(ellipse(point2D(-2, 2), size2D(0.5, 0.5))));
    UT_EXPECT(scenario, first.isCollide(ellipse(point2D(-2, 2), size2D(1, 1))));

    // With an angle of 70°, the edge is around (0.70, 1.88)
    UT_EXPECT(scenario, !first.isCollide(ellipse(point2D(1, 2), size2D(0.2, 0.1))));
    UT_EXPECT(scenario, first.isCollide(ellipse(point2D(1, 2), size2D(0.4, 0.2))));
}


int main()
{
    UnitTest::Environment tests("ellipse");

    tests.addScenario("Verify circle collision detection on X axis", VerifyCircleCollisionOnX);
    tests.addScenario("Verify circle collision detection on Y axis", verifyCircleCollisionOnY);
    tests.addScenario("Verify circle edge collision detection", verifyCircleEdgeCollisionDetection);
    tests.addScenario("Verify circle collision detection with varying radii", verifyCircleRadiiCollision);
    tests.addScenario("Verify point collision detection", verifyPointCollision);
    tests.addScenario("Verify varying radii collision detection", verifyVaryingRadiiCollisionDetection);
    tests.addScenario("Verify varying axis collision detection", verifyVaryingAxisCollisionDetection);

    return tests.run();
}