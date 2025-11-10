#include "Image.h"


Image::Image(SDL_Surface* surface) : m_surface(surface) {
	if (!m_surface)
	{
		m_surface = SDL_CreateSurface(1, 1, SDL_PIXELFORMAT_RGBA32);
	}

}