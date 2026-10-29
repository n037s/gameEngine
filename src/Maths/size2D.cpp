#include "size2D.h"

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