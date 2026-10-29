#pragma once

#include <string>

class size2D
{
public:
    explicit size2D(float w = 0, float h = 0) noexcept : w(w), h(h) {}

    size2D operator*(float mul) const noexcept;
    size2D operator/(float div) const noexcept;
    size2D operator-(const size2D& other) const noexcept;
    size2D operator*(const size2D& other) const noexcept;
    std::string toString() const;

    float w{ 0 };
    float h{ 0 };
};