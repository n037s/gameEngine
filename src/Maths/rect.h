#pragma once

#include "point2D.h"
#include "size2D.h"

class rect
{
public:
    rect() {}
    rect(const point2D& pos, const size2D& size, int d = 0) : m_position(pos), m_size(size), depth(d) {}

    const point2D& getPosition() const { return m_position; }
    void setPosition(const point2D& pos) { m_position = pos; }

    const size2D& getSize() const { return m_size; }
    void setSize(const size2D& size) { m_size = size; }

    const int getDepth() const { return depth; }
    void setDepth(int d) { depth = d; }

    /**
     * \brief Checks if this rectangle collides with another rectangle.
     * Using AABB (Axis-Aligned Bounding Box) collision detection.
     * \param other The other rectangle to check collision with.
     * \return True if the rectangles collide, false otherwise.
     */
    const bool isCollide(const rect& other) const;

private:
    point2D m_position{ 0, 0 };
    size2D m_size{ 0, 0 };
    int depth{ 0 };
};