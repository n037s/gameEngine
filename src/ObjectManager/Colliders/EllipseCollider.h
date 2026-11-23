#pragma once

#include "BaseCollider.h"

class EllipseCollider : public BaseCollider
{
public :

    EllipseCollider() {}
    ~EllipseCollider() {}

    const ColliderType getType() const override { return ColliderType::Ellipse; }
    const bool collides(const BaseCollider& other) const override;

    size2D getRadius() const { return radius; }
    void setRadius(const size2D& r) { radius = r; }

    int getHeight() const { return height; }
    void setHeight(int h) { height = h; }

private:
    size2D radius{ 0, 0 };
    int height{ 0 };
};