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
	virtual ~BaseRenderer() = default;

	void cleanSurfaceAndRenderer();
	void cleanRenderer();

	bool isGenerated() const { return m_isGenerated; }
	virtual bool render(const SDL_FRect& renderingRect);

	virtual size2D getSize() const;

	SDL_Texture* getTexture() const { return m_texture; }
	SDL_Surface* getSurface() const { return m_surface; }

protected:
	SDL_Renderer* m_renderer;
	SDL_Texture* m_texture{ nullptr };
	SDL_Surface* m_surface{ nullptr };
	bool m_isGenerated;
};


