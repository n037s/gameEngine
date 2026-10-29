#pragma once

#include "point2D.h"

class vector2D : public point2D
{
public:
    vector2D(float x = 0, float y = 0) noexcept : point2D(x, y) {}

    float length() const noexcept;
    vector2D normalized() const noexcept;
    vector2D operator*(float mul) const noexcept;
    vector2D operator/(float div) const noexcept;

private:
};