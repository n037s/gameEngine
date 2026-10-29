
#pragma once

#include "SDL3/SDL.h"
#include <string>

class size2D;
class vector2D;

class point2D
{
public:
    point2D(float x = 0, float y = 0) noexcept : x(x), y(y) {}

    point2D operator+(float add) const noexcept;
    point2D operator-(float substr) const noexcept;
    point2D operator+(const point2D& other) const noexcept;
    point2D operator+(const size2D& other) const noexcept;
    vector2D operator-(const point2D& other) const noexcept;
    point2D operator-(const size2D& other) const noexcept;
    point2D operator*(const size2D& other) const noexcept;
    point2D operator/(const size2D& other) const noexcept;
    std::string toString() const;

    SDL_FPoint toSDL() const noexcept { return SDL_FPoint{ x, y }; }
    vector2D toVector() const noexcept;

    float x;
    float y;
};