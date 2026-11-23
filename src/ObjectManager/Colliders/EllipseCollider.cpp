#include "EllipseCollider.h"

const bool EllipseCollider::collides(const BaseCollider& other) const
{
    auto otherCollider = dynamic_cast<const EllipseCollider*>(&other);
    
    auto shape = ellipse(this->getPosition(), this->getRadius());
    auto otherShape = ellipse(otherCollider->getPosition(), otherCollider->getRadius());
    return shape.isCollide(otherShape);
}

