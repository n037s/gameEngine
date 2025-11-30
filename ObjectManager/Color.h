#pragma once
#include "SDL3/SDL.h"
#include <iostream>
#include <string>

class Color
{
public:
	Color() : r(0), g(0), b(0), a(255) {};
	Color(unsigned char r, unsigned char g, unsigned char b) : r(r), g(g), b(b), a(255) {};
	Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) : r(r), g(g), b(b), a(a) {};

	SDL_Color toSDL();
	std::string toString();


	unsigned char r; 
	unsigned char g;
	unsigned char b;
	unsigned char a;
private:
};

