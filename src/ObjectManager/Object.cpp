#include "Object.h"

Object::Object(const ObjectMemberHolder& members)
{
	m_uid = members.getMember<std::string>("UID");
	m_size = members.getMember<size2D>("objectSize");
	m_pos = members.getMember<point2D>("objectPosition");
	m_z = members.getMember<float>("objectHeight");
	m_isHidden = members.getMember<bool>("isObjectHidden");
	m_steady = members.getMember<bool>("isObjectSteady", false);
}


ObjectMemberHolder Object::serialize() const
{
	ObjectMemberHolder memberHolder;
	memberHolder.addMember("UID", m_uid);
	memberHolder.addMember("objectPosition", m_pos);
	memberHolder.addMember("objectHeight", m_z);
	memberHolder.addMember("objectSize", m_size);
	memberHolder.addMember("isObjectHidden", m_isHidden);
	memberHolder.addMember("isObjectSteady", m_steady);

	return memberHolder;
}

Object::~Object()
{
    delete m_renderer;
}

bool Object::render()
{
	if (!m_renderer)
		return false;
	if (m_useRenderingQuad)
		return m_renderer->renderQuad(m_renderingCorners);
	bool res = false;
	const SDL_FRect renderingRect = m_renderingRect.toSDL();
	res = m_renderer->render(renderingRect);
	return res;
}

void Object::update()
{
	// do nothing
}

void Object::onHover(point2D pos)
{
	// do nothing
}

void Object::offHover(point2D pos)
{
	// do nothing
}

void Object::hover(point2D pos) 
{ 
	if (!m_isHovered)
	{
		std::cout << "somthing is hovered" << std::endl;
		m_isHovered = true;
		onHover(pos);
	}
}
void Object::leftFocus(point2D pos)
{
	if (m_isHovered)
	{
		m_isHovered = false;
		offHover(pos);
	}
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

bool Object::onRightClick(point2D pos)
{
	m_isRightClicked = true;
	return rightClick(pos);
}
bool Object::offRightClick(point2D pos)
{
	m_isRightClicked = false;
	return releaseRightClick(pos);
}

bool Object::rightClick(point2D pos)
{
	return false;
}
bool Object::releaseRightClick(point2D pos)
{
	return false;
}

bool Object::scroll(float dz)
{
	return false;
}
