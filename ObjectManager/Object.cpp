#include "Object.h"

Object::Object(ObjectMemberHolder members)
{
	m_pos = members.getMember<point2D>("objectPosition");
	m_z = members.getMember<float>("objectHeight");
	m_size = members.getMember<size2D>("objectSize");
	m_isHidden = members.getMember<bool>("isObjectHidden");
}


ObjectMemberHolder Object::serialize() const
{
	ObjectMemberHolder memberHolder;
	memberHolder.addMember("objectPosition", m_pos);
	memberHolder.addMember("objectHeight", m_z);
	memberHolder.addMember("objectSize", m_size);
	memberHolder.addMember("isObjectHidden", m_isHidden);

	return memberHolder;
}

bool Object::render()
{
	SDL_FRect renderingRect = m_renderingRect.toSDL();
	return m_renderer->render(renderingRect);
}

void Object::update()
{
	// do nothing
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
bool Object::offLeftClick(point2D pos)
{
	m_isLeftClicked = false;
	return releaseLeftClick(pos);
}

bool Object::leftClick(point2D pos)
{
	// do nothing
	return false;
}
bool Object::releaseLeftClick(point2D pos)
{
	// do nothing
	return false;
}

