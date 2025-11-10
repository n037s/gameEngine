#include "TextRenderer.h"

#include <iostream>


TextRenderer::TextRenderer(SDL_Renderer* renderer, const std::string& text, size2D size, Font* font, SDL_Color& color)
{
	m_renderer = renderer;
	m_size = size;
	m_color = color;
	m_text = text;
	m_font = font;

	generateTexture();
}

TextRenderer::~TextRenderer()
{
	cleanSurfaceAndRenderer();
}

void TextRenderer::generateTexture()
{
	cleanSurfaceAndRenderer();
	if (!m_font->isGenerated())
	{
		std::cout << "Error loading font: " << SDL_GetError() << '\n';
	}
	else
	{
		m_surface = SDL_CreateSurface(m_size.w, m_size.h, SDL_PIXELFORMAT_RGBA8888);
		SDL_Surface* textSurface = TTF_RenderText_Blended(m_font->toSDL(), m_text.c_str(), 0, m_color);
		if (!m_surface && !textSurface)
		{
			std::cout << "Error generating text surface" << SDL_GetError() << std::endl;
		}
		else
		{
			if (setupTextInSurface(textSurface))
			{
				m_texture = SDL_CreateTextureFromSurface(m_renderer, m_surface);
				if (!m_surface)
				{
					std::cout << "Error generating text texture" << SDL_GetError() << std::endl;
				}
				else
				{
					m_isGenerated = true;
				}
			}
		}
	}
}

void TextRenderer::setText(std::string& text)
{
	m_text = text;
	generateTexture();
}

void TextRenderer::setColor(SDL_Color& color)
{
	m_color = color;
	generateTexture();
}

void TextRenderer::setFont(std::string& font)
{
	m_font->setFontPath(font);
	generateTexture();
}

void TextRenderer::setPoliceSize(float size)
{
	m_font->setPolicySize(size);
	generateTexture();
}

bool TextRenderer::setupTextInSurface(SDL_Surface* textSurface)
{
	// Compute the position and mix the text surface to the main surface
	size2D textSurfaceSize = size2D(textSurface->w, textSurface->h);
	size2D surfaceSize = size2D(m_surface->w, m_surface->h);

	size2D oversize = surfaceSize - textSurfaceSize;
	point2D offset = point2D(0, 0);
	
	TextAlignement textAlignement = m_font->getTextAlignement();
	// we have an offset to compute according to font alignement
	offset.x = oversize.w * ( textAlignement.horizontalAlignement / 2.0);
	offset.y = oversize.h * ( textAlignement.verticalAlignement / 2.0);

	SDL_Rect textRect = rect2D(point2D(0, 0), textSurfaceSize).tointSDL();
	SDL_Rect surfaceRect = rect2D(offset, surfaceSize).tointSDL();

	bool success = SDL_BlitSurface(textSurface, &textRect, m_surface, &surfaceRect);
	if (!success)
		std::cout << "[ERROR] Generating text, blit is unsuccessfull" << std::endl;

	return success;
}