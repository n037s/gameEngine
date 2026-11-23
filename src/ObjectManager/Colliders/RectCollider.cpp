#include "RectCollider.h"

const bool RectCollider::collides(const BaseCollider& other) const
{
    auto& otherRect = static_cast<const RectCollider&>(other);

    auto shape = rect(getPosition(), getSize(), getDepth());
    auto other_shape = rect(otherRect.getPosition(), otherRect.getSize(), otherRect.getDepth());

    if (shape.isCollide(other_shape))
        return true;
    return false;
}