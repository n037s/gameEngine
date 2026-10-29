#include "point2D.h"

#include "size2D.h"
#include "vector2D.h"

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