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

	if (textSurface)
	{
		for (int row = 0; row < m_surface->h; ++row)
		{
			for (int column = 0; column < m_surface->w; ++column)
			{
				Uint8 red, green, blue, alpha;
				if (!SDL_ReadSurfacePixel(textSurface, column, row, &red, &green, &blue, &alpha) || alpha == 0)
					continue;
				const float textAlpha = alpha / 255.0f;
				const float backgroundAlpha = m_color.a / 255.0f * (1.0f - textAlpha);
				const float combinedAlpha = textAlpha + backgroundAlpha;
				auto blendChannel = [textAlpha, backgroundAlpha, combinedAlpha](Uint8 text, Uint8 background) {
					return static_cast<Uint8>((text * textAlpha + background * backgroundAlpha) / combinedAlpha + 0.5f);
				};
				SDL_WriteSurfacePixel(m_surface, column, row,
					blendChannel(red, m_color.r), blendChannel(green, m_color.g), blendChannel(blue, m_color.b),
					static_cast<Uint8>(combinedAlpha * 255.0f + 0.5f));
			}
		}
	}

	m_texture = SDL_CreateTextureFromSurface(m_renderer, m_surface);
}

void  ButtonRenderer::setColor(SDL_Color color)
{
	m_color = color;
	generateTexture();
}