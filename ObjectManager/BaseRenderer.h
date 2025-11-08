#pragma once

#include "SDL3/SDL.h"

/*
* Bases of renderers
*/
class BaseRenderer
{
public:
	BaseRenderer() : m_renderer(nullptr), m_isGenerated(true) {};
	BaseRenderer(SDL_Renderer* renderer) : m_renderer(renderer), m_isGenerated(true) {};

	bool isGenerated() { return m_isGenerated; };
	virtual bool render(SDL_FRect& renderingRect) = 0;

protected:
	SDL_Renderer* m_renderer;
	SDL_Texture* m_texture{ nullptr };
	bool m_isGenerated;
};


