#pragma once

#include "point2D.h"
#include "size2D.h"

class ellipse
{
public:
    ellipse();
    ellipse(const point2D& center, const size2D& radii);

    const point2D& getCenter() const { return center; }
    void setCenter(const point2D& center) { this->center = center; }

    const size2D& getRadii() const { return radii; }
    void setRadii(const size2D& radii) { this->radii = radii; }

    /**
     * \brief Checks whether the two axis-aligned ellipses overlap or touch in 2D.
     * \param other The other ellipse to check collision with.
     * \return True if the ellipses collide, false otherwise.
     */
    const bool isCollide(const ellipse& other) const;

private:
    point2D center;
    size2D radii;
};