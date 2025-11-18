#include "Color.h"


SDL_Color Color::toSDL()
{
	return { r, g, b, a };
}
