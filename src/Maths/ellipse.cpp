#include "ellipse.h"

ellipse::ellipse()
    : center(point2D()), radii(size2D())
{
}

ellipse::ellipse(const point2D& center, const size2D& radii)
    : center(center), radii(radii)
{
}

const bool ellipse::isCollide(const ellipse& other) const
{
    // Let d be the difference between the centers and (a, b), (c, d)
    // the two pairs of radii. All calculations below use squared radii.
    const double dx = static_cast<double>(center.x) - other.center.x;
    const double dy = static_cast<double>(center.y) - other.center.y;
    const double radiusX = radii.w;
    const double radiusY = radii.h;
    const double otherRadiusX = other.radii.w;
    const double otherRadiusY = other.radii.h;
    const double radiusX2 = radiusX * radiusX;
    const double radiusY2 = radiusY * radiusY;
    const double otherRadiusX2 = otherRadiusX * otherRadiusX;
    const double otherRadiusY2 = otherRadiusY * otherRadiusY;

    // If both shapes have zero extent on an axis, their coordinates on
    // that axis must match. This also handles points and line segments.
    if ((radiusX2 == 0 && otherRadiusX2 == 0 && dx != 0) ||
        (radiusY2 == 0 && otherRadiusY2 == 0 && dy != 0))
        return false;

    // For each t in (0, 1), minimize t * E1(p) + (1 - t) * E2(p)
    // over all points p, where E1 and E2 are the usual ellipse equations
    // ((x - cx) / rx)^2 + ((y - cy) / ry)^2. The minimum is:
    // t(1-t) * (dx^2 / ((1-t)a^2 + t*c^2)
    //          + dy^2 / ((1-t)b^2 + t*d^2)).
    // The ellipses overlap or touch exactly when the maximum of these
    // minima is <= 1. Zero radii are treated as the limit of thin ellipses.
    const auto contactValue = [&](double t) {
        const double xDenominator = (1 - t) * radiusX2 + t * otherRadiusX2;
        const double yDenominator = (1 - t) * radiusY2 + t * otherRadiusY2;
        const double xTerm = xDenominator == 0 ? 0 : dx * dx / xDenominator;
        const double yTerm = yDenominator == 0 ? 0 : dy * dy / yDenominator;
        return t * (1 - t) * (xTerm + yTerm);
    };

    // The contact value is concave, so ternary search finds its maximum.
    // A fixed number of iterations gives a double-precision approximation.
    double lower = 0;
    double upper = 1;
    for (int iteration = 0; iteration < 64; ++iteration)
    {
        const double left = lower + (upper - lower) / 3;
        const double right = upper - (upper - lower) / 3;
        if (contactValue(left) < contactValue(right))
            lower = left;
        else
            upper = right;
    }

    return contactValue((lower + upper) / 2) <= 1;
}