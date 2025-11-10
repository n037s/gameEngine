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

bool Object::onLeftClick(point2D pos)
{
	m_isLeftClicked = true;
	return leftClick(pos);
}
void Object::offLeftClick(point2D pos)
{
	m_isLeftClicked = false;
	releaseLeftClick(pos);
}

bool Object::leftClick(point2D pos)
{
	// do nothing
	return false;
}
void Object::releaseLeftClick(point2D pos)
{
	// do nothing
}