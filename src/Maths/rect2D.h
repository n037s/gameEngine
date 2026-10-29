#pragma once

#include <string>
#include "point2D.h"
#include "size2D.h"

struct SDL_FRect;
struct SDL_Rect;

class rect2D
{
public:
    rect2D(float x = 0, float y = 0, float w = 0, float h = 0) noexcept;
    rect2D(const point2D& topLeft = { 0,0 }, const size2D& size = size2D{ 0,0 }) noexcept;
    std::string toString() const;

    bool isInside(const point2D& point) const noexcept;
    bool isCollide(const rect2D& rect) const noexcept;

    SDL_FRect toSDL() const noexcept;
    SDL_Rect tointSDL() const noexcept;

    point2D position{ 0, 0 };
    size2D size{ 0,0 };
};