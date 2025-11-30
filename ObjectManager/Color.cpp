#include "Color.h"


SDL_Color Color::toSDL()
{
	return { r, g, b, a };
}

std::string Color::toString()
{
	return "(" + std::to_string(r) + ";" + std::to_string(g) + ";" +
		std::to_string(b) + ";" + std::to_string(a) + ")";
}