#include "Text.h"

#include <iostream>

#include "TextRenderer.h"
#include "AssetsManager.h"

Text::Text(rect2D shape, const std::string& text, const std::string& font, float fontSize, Color color) : Object(shape.position, shape.size)
{
	m_text = text;
	m_font = font;
	m_fontSize = fontSize;
	m_color = color;
}

bool Text::createRenderer(SDL_Renderer* renderer)
{
	m_renderer = new TextRenderer(renderer, m_text, m_size, std::make_shared<Font>(m_font, m_fontSize), m_color.toSDL());
	m_size = m_renderer->getSize();

	return m_renderer->isGenerated();
}

Text::Text(const ObjectMemberHolder& members) : Object(members)
{
	m_text = members.getMember<std::string>("textLabel");
	m_font = members.getMember<std::string>("textFont");
	m_fontSize = members.getMember<float>("textFontSize");
	m_color = members.getMember<Color>("textColor");
}

ObjectMemberHolder Text::serialize() const
{
	ObjectMemberHolder objectMemberHolder = Object::serialize();
	objectMemberHolder.addMember("textLabel", m_text);
	objectMemberHolder.addMember("textFont", m_font);
	objectMemberHolder.addMember("textFontSize", m_fontSize);
	objectMemberHolder.addMember("textColor", m_color);
	return objectMemberHolder;
}

bool Text::setText(const std::string& text)
{
	bool success = false;
	TextRenderer* textRend = static_cast<TextRenderer*>(m_renderer);
	if (textRend)
	{
		textRend->setText(text);
		success = textRend->isGenerated();
	}
	return success;
}

bool Text::setColor(Color color)
{
	bool success = false;
	TextRenderer* textRend = static_cast<TextRenderer*>(m_renderer);
	if (textRend)
	{
		textRend->setColor(color.toSDL());
		success = textRend->isGenerated();
	}
	return success;
}

bool Text::setFont(const std::string& font)
{
	bool success = false;
	TextRenderer* textRend = static_cast<TextRenderer*>(m_renderer);
	if (textRend)
	{
		std::string fontPath = m_font + font;
		textRend->setFont(fontPath);
		success = textRend->isGenerated();
	}
	return success;
}

void Text::setPoliceSize(float size)
{
	TextRenderer* textRend = static_cast<TextRenderer*>(m_renderer);
	if (textRend)
	{
		textRend->setPoliceSize(size);
	}
}