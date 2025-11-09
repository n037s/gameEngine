#include "Button.h"

#include "ButtonRenderer.h"

#include <iostream>

Button::Button(point2D pos, std::string text, buttonFunction callback)
{
	m_pos = pos;
	m_label = text;
	m_callback = callback;
}

void Button::onClick()
{
	m_callback();
}

bool Button::createRenderer(SDL_Renderer* renderer)
{
	m_renderer = new ButtonRenderer({ 0,0 }, renderer);
	m_size = m_renderer->getSize();

	return m_renderer->isGenerated();
}