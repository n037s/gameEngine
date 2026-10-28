#include "DropDownMenuRenderer.h"

#include "ButtonRenderer.h"

#include <iostream>

DropDownMenuRenderer::DropDownMenuRenderer(SDL_Renderer* renderer, size2D size, size2D itemSize, SDL_Color backgroundColor)
{
	m_renderer = renderer;
	m_size = size;
	m_itemSize = itemSize;
	m_backgroundColor = backgroundColor;
}

DropDownMenuRenderer::~DropDownMenuRenderer()
{
	cleanSurfaceAndRenderer();
}

void DropDownMenuRenderer::clearItems()
{
	m_objects.clear();
}

void DropDownMenuRenderer::addItem(ButtonRenderer* object, point2D position)
{
	m_objects.emplace_back(object, position);
}

void DropDownMenuRenderer::update()
{
	cleanSurfaceAndRenderer();
	m_surface = SDL_CreateSurface(static_cast<int>(m_size.w), static_cast<int>(m_size.h), SDL_PIXELFORMAT_RGBA8888);
	const point2D rel_pos(0, 0);
	const SDL_Rect brect = { static_cast<int>(rel_pos.x), static_cast<int>(rel_pos.y), static_cast<int>(m_size.w), static_cast<int>(m_size.h) };
	SDL_FillSurfaceRect(m_surface, &brect, SDL_MapSurfaceRGBA(m_surface, 0, 0, 0, 0));

	int i = 0;
	for (auto object : m_objects)
	{
		auto const objSurface = object.first->getSurface();
		if (!objSurface)
			continue;

		const SDL_Rect surfaceShape = rect2D(0, 0, objSurface->w, objSurface->h).tointSDL();
		const SDL_Rect rel_pos = rect2D(object.second, object.first->getSize()).tointSDL();

		bool success = SDL_BlitSurface(objSurface, &(surfaceShape), m_surface, &(rel_pos));
		if (!success)
			std::cout << "[ERROR] Button renderer - blit is not successfull" << std::endl;
		else
			i++;
	}

	m_texture = SDL_CreateTextureFromSurface(m_renderer, m_surface);
}