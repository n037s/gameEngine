#include "ButtonRenderer.h"

#include "TextRenderer.h"

#include <iostream>

ButtonRenderer::ButtonRenderer(SDL_Renderer* renderer, size2D size, SDL_Color color, const std::string& label, FontPtr font, SDL_Color fontColor)
{
	m_renderer = renderer;
	m_size = size;
	m_color = color;
	m_label = label;
	m_font = font;
	m_fontColor = fontColor;

	generateTexture();
}

ButtonRenderer::~ButtonRenderer()
{
	cleanSurfaceAndRenderer();
}

void ButtonRenderer::generateTexture()
{
	cleanSurfaceAndRenderer();
	m_surface = SDL_CreateSurface(static_cast<int>(m_size.w), static_cast<int>(m_size.h), SDL_PIXELFORMAT_RGBA8888);
	const point2D rel_pos(0, 0);
	const SDL_Rect brect = { static_cast<int>(rel_pos.x), static_cast<int>(rel_pos.y), static_cast<int>(m_size.w), static_cast<int>(m_size.h) };
	SDL_FillSurfaceRect(m_surface, &brect, SDL_MapSurfaceRGBA(m_surface, m_color.r, m_color.g, m_color.b, m_color.a));

	TextRenderer const textRend = TextRenderer(m_renderer, m_label, m_size, m_font, m_fontColor);

	auto textSurface = textRend.getSurface();

	bool success = SDL_BlitSurface(textSurface, &brect, m_surface, &brect);
	if (!success)
		std::cout << "[ERROR] Button renderer - blit is not successfull" << std::endl;

	m_texture = SDL_CreateTextureFromSurface(m_renderer, m_surface);
}

void  ButtonRenderer::setColor(SDL_Color color)
{
	m_color = color;
	generateTexture();
}