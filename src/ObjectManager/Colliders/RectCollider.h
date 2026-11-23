#pragma once

#include "BaseCollider.h"

class RectCollider : public BaseCollider
{
public :

    RectCollider() {}
    ~RectCollider() {}

    const ColliderType getType() const override { return ColliderType::Rectangle; }
    const bool collides(const BaseCollider& other) const override;

    const size2D& getSize() const { return m_size; }
    void setSize(const size2D& s) { m_size = s; }

    int getDepth() const { return depth; }
    void setDepth(int d) { depth = d; }

private:
    size2D m_size{ 0, 0 };
    int depth{ 0 };
    
};
    