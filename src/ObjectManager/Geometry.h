#pragma once

#include "SDL3/SDL.h"
#include <string>

class size2D
{
public:
    size2D(float w = 0, float h = 0) noexcept : w(w), h(h) {}

    size2D operator*(float mul) const noexcept;
    size2D operator/(float div) const noexcept;
    size2D operator-(const size2D& other) const noexcept;
    size2D operator*(const size2D& other) const noexcept;
    std::string toString() const;

    float w{ 0 };
    float h{ 0 };
};

class point2D
{
public:
    point2D(float x = 0, float y = 0) noexcept : x(x), y(y) {}

    point2D operator+(float add) const noexcept;
    point2D operator-(float substr) const noexcept;
    point2D operator*(float mul) const noexcept;
    point2D operator/(float div) const noexcept;
    point2D operator+(const point2D& other) const noexcept;
    point2D operator+(const size2D& other) const noexcept;
    point2D operator-(const point2D& other) const noexcept;
    point2D operator-(const size2D& other) const noexcept;
    point2D operator*(const size2D& other) const noexcept;
    point2D operator/(const size2D& other) const noexcept;
    std::string toString() const;

    SDL_FPoint toSDL() const noexcept { return SDL_FPoint{ x, y }; }

    float x;
    float y;
};

class rect2D
{
public:
    rect2D(float x = 0, float y = 0, float w = 0, float h = 0) noexcept
        : position(point2D{ x, y }), size(size2D{ w, h }) {}
    rect2D(const point2D& topLeft = { 0,0 }, const size2D& size = { 0,0 }) noexcept
        : position(topLeft), size(size) {}
    std::string toString() const;

    bool isInside(const point2D& point) const noexcept;
    bool isCollide(const rect2D& rect) const noexcept;

    SDL_FRect toSDL() const noexcept { return SDL_FRect{ position.x, position.y, size.w, size.h }; }
    SDL_Rect tointSDL() const noexcept { return SDL_Rect{ static_cast<int>(position.x), static_cast<int>(position.y), static_cast<int>(size.w), static_cast<int>(size.h) }; }

    point2D position{ 0, 0 };
    size2D size{ 0,0 };
};



