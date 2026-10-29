#include "rect2D.h"

#include "SDL3/SDL.h"


rect2D::rect2D(float x, float y, float w, float h) noexcept
    : position(point2D{ x, y }), size(size2D{ w, h }) 
{

}

rect2D::rect2D(const point2D& topLeft, const size2D& size) noexcept
    : position(topLeft), size(size) 
{

}

std::string rect2D::toString() const 
{ 
    return "[ " + position.toString() + " - " + size.toString() + " ]"; 
}

bool rect2D::isInside(const point2D& point) const noexcept
{
    point2D low_bound = position;
    point2D high_bound = position + size;
    return ((point.x > low_bound.x) && (point.x < high_bound.x) &&
            (point.y > low_bound.y) && (point.y < high_bound.y));
}

bool rect2D::isCollide(const rect2D& rect) const noexcept
{
    point2D low_bound = position;
    point2D high_bound = position + size;
    point2D low_other_bound = rect.position;
    point2D high_other_bound = rect.position + rect.size;

    return ((low_bound.x < high_other_bound.x) && (high_bound.x > low_other_bound.x)) &&
           ((low_bound.y < high_other_bound.y) && (high_bound.y > low_other_bound.y));
}

SDL_FRect rect2D::toSDL() const noexcept
{
    return SDL_FRect{ position.x, position.y, size.w, size.h };
}

SDL_Rect rect2D::tointSDL() const noexcept
{ 
    return SDL_Rect{ static_cast<int>(position.x), static_cast<int>(position.y), static_cast<int>(size.w), static_cast<int>(size.h) }; 
}