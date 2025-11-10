#include "Button.h"

#include "ButtonRenderer.h"

#include <iostream>

Button::Button(rect2D shape, std::string text, SDL_Color& color, Font* font, SDL_Color& fontColor, 
	buttonFunction callback, buttonFunction releaseCallback)
{
	m_pos = shape.position;
	m_label = text;
	m_size = shape.size;
	m_color = color;
	m_font = font;
	m_fontColor = fontColor;
	m_callback = callback;
	m_releaseCallback = releaseCallback;
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

bool Button::leftClick(point2D pos)
{
	bool success = false;
	if (m_callback != nullptr)
		success = m_callback(pos);
	return success;
}

void Button::releaseLeftClick(point2D pos)
{
	if (m_releaseCallback != nullptr)
		m_releaseCallback(pos);
}

bool Button::createRenderer(SDL_Renderer* renderer)
{
	m_renderer = new ButtonRenderer(renderer, m_size, m_color, m_label, m_font, m_fontColor);
	m_size = m_renderer->getSize();

	return m_renderer->isGenerated();
}