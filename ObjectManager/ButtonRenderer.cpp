#include "ButtonRenderer.h"

#include "TextRenderer.h"

#include <iostream>


ButtonRenderer::ButtonRenderer(const SDL_FPoint pos, SDL_Renderer* renderer)
{
	// Here we define the button, its size and its look
	m_renderer = renderer;
	generateTexture();
}

ButtonRenderer::~ButtonRenderer()
{
	// Destroy the texture
}

void ButtonRenderer::generateTexture()
{
	size2D size{ 100,25 };
	m_surface = SDL_CreateSurface(size.w, size.h, SDL_PIXELFORMAT_RGBA8888);
	SDL_Rect brect = { 0, 0, size.w, size.h };
	SDL_FillSurfaceRect(m_surface, &brect, SDL_MapSurfaceRGB(m_surface, 255, 255, 255));

	point2D pos(0, 0);
	std::string label = "button";
	std::string fontPath = "C:\\Users\\leoqu\\Desktop\\Code\\Project\\Assets\\arial.ttf";
	SDL_Color color = { 50, 50, 50, 255 };
	TextRenderer* textRend = new TextRenderer(pos.toSDL(), label, fontPath, 25.0, m_renderer, color);

	auto textSurface = textRend->getSurface();

	bool success = SDL_BlitSurface(textSurface, &brect, m_surface, &brect);
	if (success)
		std::cout << "blit is successfull" << std::endl;
	else
		std::cout << "blit not successfull : " << SDL_GetError() << std::endl;

	m_texture = SDL_CreateTextureFromSurface(m_renderer, m_surface);
}