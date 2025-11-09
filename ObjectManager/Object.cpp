#include "Object.h"

bool Object::render()
{
	SDL_FRect renderingRect = m_renderingRect.toSDL();
	return m_renderer->render(renderingRect);
}

void Object::onHoover()
{
	m_isHoovered = true;
	hoover();
}

void Object::offHoover()
{
	m_isHoovered = false;
	leftFocus();
}

void Object::hoover() 
{ 
	// do nothing
}
void Object::leftFocus()
{
	// do nothing
}
void Object::onClick()
{
	// do nothing
}