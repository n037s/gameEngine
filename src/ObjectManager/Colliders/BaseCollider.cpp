#include "BaseCollider.h"
#include "EllipseCollider.h"
#include "RectCollider.h"

#include <algorithm>
#include <cmath>

bool manageRectangle(const RectCollider& a, const BaseCollider& b);
bool manageEllipse(const EllipseCollider& a, const BaseCollider& b);
bool collides(const RectCollider& a, const EllipseCollider& b);

BaseCollider::BaseCollider() {}
BaseCollider::~BaseCollider() {}

bool collides(const BaseCollider& a, const BaseCollider& b)
{
    // Manages collition between objects

    if (a.getType() == b.getType())
    {
        // Call inner method to verify collision between same items 
        return a.collides(b);
    }
    else
    {
        switch (a.getType())
        {
        case ColliderType::Rectangle:
            return manageRectangle(static_cast<const RectCollider&>(a), b);
        case ColliderType::Ellipse:
            return manageEllipse(static_cast<const EllipseCollider&>(a), b);
        default:
            return false;
        }
    }
}

// ---------------------------------------------------------------------
// -------------------------- Colliders logics -------------------------
// ---------------------------------------------------------------------

bool manageRectangle(const RectCollider& a, const BaseCollider& b)
{
    switch (b.getType())
    {
        case ColliderType::Ellipse:
            return collides(a, static_cast<const EllipseCollider&>(b));
        default :
            return false;
    }
}

bool manageEllipse(const EllipseCollider& a, const BaseCollider& b)
{
    switch (b.getType())
    {
        case ColliderType::Rectangle:
            return collides(static_cast<const RectCollider&>(b), a);
        default :
            return false;
    }
}

// ---------------------------------------------------------------------
// -------------------------- Colliders methods ------------------------
// ---------------------------------------------------------------------

bool collides(const RectCollider& a, const EllipseCollider& b)
{
    const double left = a.getPosition().x;
    const double top = a.getPosition().y;
    const double right = left + a.getSize().w;
    const double bottom = top + a.getSize().h;
    const double centerX = b.getPosition().x;
    const double centerY = b.getPosition().y;
    const double radiusX = std::abs(static_cast<double>(b.getRadius().w));
    const double radiusY = std::abs(static_cast<double>(b.getRadius().h));
    const double dx = centerX - std::clamp(centerX, left, right);
    const double dy = centerY - std::clamp(centerY, top, bottom);

    if ((radiusX == 0 && dx != 0) || (radiusY == 0 && dy != 0))
        return false;

    const double normalizedX = radiusX == 0 ? 0 : dx / radiusX;
    const double normalizedY = radiusY == 0 ? 0 : dy / radiusY;
    return normalizedX * normalizedX + normalizedY * normalizedY <= 1;
}