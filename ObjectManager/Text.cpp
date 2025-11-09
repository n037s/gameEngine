#include "Text.h"

#include <iostream>

#include "TextRenderer.h"

const std::string assetPath = "C:\\Users\\leoqu\\Desktop\\Code\\Project\\Assets\\";

Text::Text(point2D pos, std::string text, std::string font, float fontSize, SDL_Color color)
{
	m_pos = pos;
	m_text = text;
	m_font = assetPath + font;
	m_fontSize = fontSize;
	m_color = color;
}

bool Text::createRenderer(SDL_Renderer* renderer)
{
	m_renderer = new TextRenderer(m_pos.toSDL(), m_text, m_font, m_fontSize, renderer, m_color);
	m_size = m_renderer->getSize();

	return m_renderer->isGenerated();
}

bool Text::setText(std::string text)
{
	bool success = false;
	TextRenderer* textRend = static_cast<TextRenderer*>(m_renderer);
	if (textRend)
	{
		textRend->setText(text);
		success = textRend->isGenerated();
	}
	return success;
}

bool Text::setColor(SDL_Color color)
{
	bool success = false;
	TextRenderer* textRend = static_cast<TextRenderer*>(m_renderer);
	if (textRend)
	{
		textRend->setColor(color);
		success = textRend->isGenerated();
	}
	return success;
}

bool Text::setFont(std::string font)
{
	bool success = false;
	TextRenderer* textRend = static_cast<TextRenderer*>(m_renderer);
	if (textRend)
	{
		std::string fontPath = m_font + font;
		textRend->setFont(fontPath);
		success = textRend->isGenerated();
	}
	return success;
}

void Text::setPoliceSize(float size)
{
	TextRenderer* textRend = static_cast<TextRenderer*>(m_renderer);
	if (textRend)
	{
		textRend->setPoliceSize(size);
	}
}