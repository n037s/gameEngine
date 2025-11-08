#include "Geometry.h"
#include <iostream>

point2D point2D::operator+(point2D other) const
{
	return point2D(this->x + other.x, this->y + other.y);
}

point2D point2D::operator+(size2D other) const
{
	return point2D(this->x + other.w, this->y + other.h);
}

point2D point2D::operator-(point2D other) const
{
	return point2D(this->x - other.x, this->y - other.y);
}

point2D point2D::operator-(size2D other) const
{
	return point2D(this->x - other.w, this->y - other.h);
}

std::string point2D::toString() const
{
	return "(" + std::to_string(x) + ";" + std::to_string(y) + ")";
}


size2D size2D::operator*(float mul) const
{
	return size2D(mul * this->w, mul * this->h);
}

size2D size2D::operator/(float div) const
{
	return size2D(this->w / div, this->h / div);
}

std::string size2D::toString() const
{
	return "(" + std::to_string(w) + "x" + std::to_string(h) + ")";
}



bool rect2D::isInside(const point2D& point)
{
	point2D low_bound = position;
	point2D high_bound = position + size;

	return ((point.x > low_bound.x) && (point.x < high_bound.x) ||
		((point.y > low_bound.y) && (point.y < high_bound.y)));
}

bool rect2D::isCollide(const rect2D& rect)
{
	point2D low_bound = position;
	point2D high_bound = position + size;
	point2D low_other_bound = rect.position;
	point2D high_other_bound = rect.position + rect.size;

	return ((low_bound.x < high_other_bound.x) && (high_bound.x > low_other_bound.x))
		&& ((low_bound.y < high_other_bound.y) && (high_bound.y > low_other_bound.y));
}

std::string rect2D::toString() const
{
	return "[ " + position.toString() + " - " + size.toString() + " ]";
}