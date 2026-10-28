#pragma once
#include "Object.h"

#include "Color.h"
#include "ButtonCallbackRegistery.h"

#include "Font.h"

#include <functional>
#include <string>

class Button : public Object
{
public:
	Button(const ObjectMemberHolder& members);
	Button(rect2D shape, const std::string& text, const Color& color, FontPtr font, const Color& fontColor,
		const std::string& callbackID = "", const std::string& releaseCallbackID = "");

	bool createRenderer(SDL_Renderer* renderer) override;

	std::string getTypeName() const override { return "Button"; }
	ObjectMemberHolder serialize() const override;

	void onHover(point2D pos) override;
	void offHover(point2D pos) override;
	bool leftClick(point2D pos) override;
	bool releaseLeftClick(point2D pos) override;

	const rect2D getShape() const override { return rect2D(m_pos, m_size); };
	
private:
	std::string m_label;
	Color m_color;
	FontPtr m_font;
	Color m_fontColor;
	std::string m_callbackID;
	std::string m_callbackReleaseID;
	buttonCallback m_callback;
	buttonCallback m_releaseCallback;
	
};

