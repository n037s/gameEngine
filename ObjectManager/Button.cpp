#include "Button.h"

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
	ButtonRender* buttonRenderer = new ButtonRender(m_pos.toSDL(), m_assetPath, renderer);
	m_renderer = buttonRenderer;
	m_size = buttonRenderer->getImageSize();

	return m_renderer->isGenerated();
}