#include "Button.h"

#include "ButtonRenderer.h"

#include <iostream>

Button::Button(rect2D shape, std::string text, SDL_Color& color, Font* font, SDL_Color& fontColor, buttonFunction callback)
{
	m_pos = shape.position;
	m_label = text;
	m_size = shape.size;
	m_color = color;
	m_font = font;
	m_fontColor = fontColor;
	m_callback = callback;
}

void Button::hoover()
{
	// Change button color 
	m_color.r += 30;
	m_color.g += 30;
	m_color.b += 30;
	static_cast<ButtonRenderer*>(m_renderer)->setColor(m_color);
}

void Button::leftFocus()
{
	// Change button color 
	m_color.r -= 30;
	m_color.g -= 30;
	m_color.b -= 30;
	static_cast<ButtonRenderer*>(m_renderer)->setColor(m_color);
}

void Button::onClick()
{
	m_callback();
}

bool Button::createRenderer(SDL_Renderer* renderer)
{
	m_renderer = new ButtonRenderer(renderer, m_size, m_color, m_label, m_font, m_fontColor);
	m_size = m_renderer->getSize();

	return m_renderer->isGenerated();
}