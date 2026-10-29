#include "vector2D.h"

#include <cmath>

float vector2D::length() const noexcept
{
    return std::sqrt(x * x + y * y);
}

vector2D vector2D::normalized() const noexcept
{
    float len = length();
    if (len > 0)
        return vector2D(x / len, y / len);
    return vector2D(0, 0);
}

vector2D vector2D::operator*(float mul) const noexcept
{
    return vector2D(x * mul, y * mul);
}

vector2D vector2D::operator/(float div) const noexcept 
{ 
    return vector2D(x / div, y / div); 
}