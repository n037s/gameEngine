#include "TextRenderer.h"

#include <iostream>


TextRenderer::TextRenderer(const SDL_FPoint& pos, const std::string& text, const std::string font, const float fontSize, SDL_Renderer* renderer, SDL_Color& color)
{
	m_renderer = renderer;
	m_color = color;
	m_text = text;
	m_policy = font;
	m_policySize = fontSize;

	m_font = TTF_OpenFont(m_policy.c_str(), m_policySize);

	generateTexture();
}

TextRenderer::~TextRenderer()
{
	if (TTF_WasInit()) {
		TTF_CloseFont(m_font);
	}
	if (m_surface)
	{
		SDL_DestroySurface(m_surface);
	}
	if (m_texture)
	{
		SDL_DestroyTexture(m_texture);
	}
}

void TextRenderer::cleanTexture()
{
	if (m_surface)
	{
		std::cout << "surface destroyed" << std::endl;
		SDL_DestroySurface(m_surface);
	}
	if (m_texture)
	{
		std::cout << "texture destroyed" << std::endl;
		SDL_DestroyTexture(m_texture);
	}
}

void TextRenderer::generateTexture()
{
	cleanTexture();
	if (!m_font)
	{
		std::cout << "Error loading font: " << SDL_GetError() << '\n';
	}
	else
	{
		m_surface = TTF_RenderText_Blended(m_font, m_text.c_str(), 0, m_color);
		if (!m_surface)
		{
			std::cout << "Error generating text surface" << SDL_GetError() << std::endl;
		}
		else
		{
			m_texture = SDL_CreateTextureFromSurface(m_renderer, m_surface);
			if (!m_surface)
			{
				std::cout << "Error generating text texture" << SDL_GetError() << std::endl;
			}
			else
			{
				std::cout << "text is generated " << m_policySize << std::endl;
				m_isGenerated = true;
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
	m_policy = font;
	TTF_CloseFont(m_font);
	m_font = TTF_OpenFont(m_policy.c_str(), m_policySize);
	generateTexture();
}

void TextRenderer::setPoliceSize(float size)
{
	m_policySize = size;
	TTF_CloseFont(m_font);
	m_font = TTF_OpenFont(m_policy.c_str(), m_policySize);
	generateTexture();
}