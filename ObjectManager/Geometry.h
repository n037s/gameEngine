#pragma once

#include "SDL3/SDL.h"
#include <string>

class size2D
{
public:
	size2D() : w(0), h(0) {};
	size2D(float w, float h) : w(w), h(h) {};

	size2D operator*(float mul) const;
	size2D operator/(float div) const; 
	size2D operator-(size2D other) const;
	std::string toString() const;

	float w{ 0 };
	float h{ 0 };
};

class point2D
{
public:
	point2D(float x, float y) : x(x), y(y) {};

	point2D operator+(point2D other) const;
	point2D operator+(size2D other) const;
	point2D operator-(point2D other) const;
	point2D operator-(size2D other) const;
	point2D operator*(size2D other) const;
	std::string toString() const;

	SDL_FPoint toSDL() { return SDL_FPoint{ x, y }; };

	float x;
	float y;
};

class rect2D
{
public:
	rect2D(float x, float y, float w, float h) : position(point2D{ x, y }), size(size2D{ w, h }) {};
	rect2D(point2D topLeft, size2D size) : position(topLeft), size(size) {};
	std::string toString() const;

	bool isInside(const point2D& point);
	bool isCollide(const rect2D& rect);

	SDL_FRect toSDL() { return SDL_FRect{ position.x, position.y, size.w, size.h }; }
	SDL_Rect tointSDL() { return SDL_Rect{ (int)position.x, (int)position.y, (int)size.w, (int)size.h }; }

	point2D position{ 0, 0 };
	size2D size{ 0,0 };
};



