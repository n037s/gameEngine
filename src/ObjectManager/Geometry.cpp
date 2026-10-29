#include "Geometry.h"
#include <iostream>
#include <cmath>

// ------------------- size2D -------------------
size2D size2D::operator*(float mul) const noexcept 
{ 
    return size2D(mul * w, mul * h); 
}

size2D size2D::operator/(float div) const noexcept 
{ 
    return size2D(w / div, h / div); 
}

size2D size2D::operator-(const size2D& other) const noexcept 
{ 
    return size2D(w - other.w, h - other.h); 
}

size2D size2D::operator*(const size2D& other) const noexcept 
{ 
    return size2D(w * other.w, h * other.h); 
}

std::string size2D::toString() const 
{ 
    return "(" + std::to_string(w) + "x" + std::to_string(h) + ")"; 
}

// ------------------- point2D -------------------
point2D point2D::operator+(float add) const noexcept 
{ 
    return point2D(x + add, y + add); 
}

point2D point2D::operator-(float substr) const noexcept 
{ 
    return point2D(x - substr, y - substr); 
}

point2D point2D::operator+(const point2D& other) const noexcept 
{ 
    return point2D(x + other.x, y + other.y); 
}

point2D point2D::operator+(const size2D& other) const noexcept 
{ 
    return point2D(x + other.w, y + other.h); 
}

vector2D point2D::operator-(const point2D& other) const noexcept 
{ 
    return vector2D(x - other.x, y - other.y); 
}

point2D point2D::operator-(const size2D& other) const noexcept 
{ 
    return point2D(x - other.w, y - other.h); 
}

point2D point2D::operator*(const size2D& other) const noexcept 
{ 
    return point2D(x * other.w, y * other.h); 
}

point2D point2D::operator/(const size2D& other) const noexcept 
{ 
    return point2D(x / other.w, y / other.h); 
}

std::string point2D::toString() const 
{ 
    return "(" + std::to_string(x) + ";" + std::to_string(y) + ")"; 
}

vector2D point2D::toVector() const noexcept
{
    return vector2D(x, y);
}

// ------------------- vector2D ----------------
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

// ------------------- rect2D -------------------
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