#include "Button.h"

#include "ButtonRenderer.h"

#include <iostream>

Button::Button(rect2D shape, const std::string& text, const Color& color, FontPtr font, const Color& fontColor,
	const std::string& callbackID, const std::string& releaseCallbackID) : Object(shape.position, shape.size)
{
	m_label = text;
	m_color = color;
	m_font = font;
	m_fontColor = fontColor;
	m_callbackID = callbackID;
	m_callbackReleaseID = releaseCallbackID;
	m_callback = ButtonCallbackRegistery::instance()->getCallback(m_callbackID);
	m_releaseCallback = ButtonCallbackRegistery::instance()->getCallback(m_callbackReleaseID);
}

Button::Button(const ObjectMemberHolder& members) : Object(members)
{
	m_label = members.getMember<std::string>("buttonLabel");
	m_color = members.getMember<Color>("buttonColor");
	m_font = members.getMember<FontPtr>("buttonLabelFont");
	m_fontColor = members.getMember<Color>("buttonLabelFontColor");
	m_callbackID = members.getMember<std::string>("butonCallbackOnClick");
	m_callback = ButtonCallbackRegistery::instance()->getCallback(m_callbackID);
	m_callbackReleaseID = members.getMember<std::string>("buttonCallbackOnRelease");
	m_releaseCallback = ButtonCallbackRegistery::instance()->getCallback(m_callbackReleaseID);
}

ObjectMemberHolder Button::serialize() const
{
	ObjectMemberHolder objectMemberHolder = Object::serialize();
	objectMemberHolder.addMember("buttonLabel", m_label);
	objectMemberHolder.addMember("buttonColor", m_color);
	objectMemberHolder.addMember("buttonLabelFont", m_font);
	objectMemberHolder.addMember("buttonLabelFontColor", m_fontColor);
	objectMemberHolder.addMember("butonCallbackOnClick", m_callbackID);
	objectMemberHolder.addMember("buttonCallbackOnRelease", m_callbackReleaseID);
	return objectMemberHolder;
}

void Button::onHover(point2D pos)
{
	// Change button color 
	m_color.r += 30;
	m_color.g += 30;
	m_color.b += 30;
	std::cout << " -> " << m_color.toString() << std::endl;
	static_cast<ButtonRenderer*>(m_renderer)->setColor(m_color.toSDL());
}

void Button::offHover(point2D pos)
{
	// Change button color 
	m_color.r -= 30;
	m_color.g -= 30;
	m_color.b -= 30;
	std::cout << " -> " << m_color.toString() << std::endl;
	static_cast<ButtonRenderer*>(m_renderer)->setColor(m_color.toSDL());
}

bool Button::leftClick(point2D pos)
{
	bool success = false;
	std::cout << "button is clicked on : " << (m_callback != nullptr) << "calling " << m_callbackID << std::endl;
	if (m_callback != nullptr)
		success = m_callback(pos, this);
	std::cout << "is successfull ? " << success << std::endl;
	return success;
}

bool Button::releaseLeftClick(point2D pos)
{
	bool success = false;
	if (m_releaseCallback != nullptr)
		success = m_releaseCallback(pos, this);
	return success;
}

bool Button::createRenderer(SDL_Renderer* renderer)
{
	m_renderer = new ButtonRenderer(renderer, m_size, m_color.toSDL(), m_label, m_font, m_fontColor.toSDL());
	m_size = m_renderer->getSize();

	return m_renderer->isGenerated();
}