#include "Object.h"

bool Object::render()
{
	SDL_FRect renderingRect = m_renderingRect.toSDL();
	return m_renderer->render(renderingRect);
}

bool Object::onClick()
{
	return false;
}