#pragma once

#include "SDL3/SDL.h"

#include "Geometry.h"

/*
* Bases of renderers
*/
class BaseRenderer
{
public:
	BaseRenderer() : m_renderer(nullptr), m_isGenerated(true) {};
	BaseRenderer(SDL_Renderer* renderer) : m_renderer(renderer), m_isGenerated(true) {};

	bool isGenerated() { return m_isGenerated; };
	virtual bool render(SDL_FRect& renderingRect);

	virtual size2D getSize();

	SDL_Texture* getTexture() { return m_texture; }
	SDL_Surface* getSurface() { return m_surface; }

protected:
	SDL_Renderer* m_renderer;
	SDL_Texture* m_texture{ nullptr };
	SDL_Surface* m_surface{ nullptr };
	bool m_isGenerated;
};


