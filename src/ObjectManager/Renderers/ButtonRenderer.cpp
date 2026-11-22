#include "ButtonRenderer.h"

#include "TextRenderer.h"
#include "Image.h"

#include <algorithm>

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

	const float previewSpace = m_preview && m_preview->getSurface() ? std::min(m_size.h, m_size.w) : 0;
	TextRenderer const textRend = TextRenderer(m_renderer, m_label, size2D(m_size.w - previewSpace, m_size.h), m_font, m_fontColor);

	auto textSurface = textRend.getSurface();

	if (textSurface)
	{
		for (int row = 0; row < m_surface->h; ++row)
		{
			for (int column = 0; column < textSurface->w; ++column)
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

	if (previewSpace > 4)
	{
		auto imageSurface = m_preview->getSurface();
		const float scale = std::min((previewSpace - 4) / imageSurface->w, (m_size.h - 4) / imageSurface->h);
		const int width = std::max(1, static_cast<int>(imageSurface->w * scale));
		const int height = std::max(1, static_cast<int>(imageSurface->h * scale));
		const SDL_Rect destination{ static_cast<int>(m_size.w - previewSpace + (previewSpace - width) / 2),
			static_cast<int>((m_size.h - height) / 2), width, height };
		SDL_BlitSurfaceScaled(imageSurface, nullptr, m_surface, &destination, SDL_SCALEMODE_NEAREST);
	}

	m_texture = SDL_CreateTextureFromSurface(m_renderer, m_surface);
}

void ButtonRenderer::setPreview(Image* preview)
{
	if (m_preview == preview)
		return;
	m_preview = preview;
	generateTexture();
}

void  ButtonRenderer::setColor(SDL_Color color)
{
	m_color = color;
	generateTexture();
}