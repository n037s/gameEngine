#include "Text.h"

#include <iostream>

#include "TextRenderer.h"
#include "AssetsManager.h"

Text::Text(rect2D shape, std::string text, std::string font, float fontSize, SDL_Color color)
{
	m_pos = shape.position;
	m_size = shape.size;
	m_text = text;
	m_font = font;
	m_fontSize = fontSize;
	m_color = color;
}

bool Text::createRenderer(SDL_Renderer* renderer)
{
	m_renderer = new TextRenderer(renderer, m_text, m_size, new Font(m_font, m_fontSize), m_color);
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