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
	Button(ObjectMemberHolder members);
	Button(rect2D shape, std::string text, Color& color, Font* font, Color& fontColor,
		std::string callbackID = "", std::string releaseCallbackID = "");

	bool createRenderer(SDL_Renderer* renderer) override;

	std::string getTypeName() const override { return "Button"; }
	ObjectMemberHolder serialize() const override;

	void onHover(point2D pos) override;
	void offHover(point2D pos) override;
	bool leftClick(point2D pos) override;
	bool releaseLeftClick(point2D pos) override;

	const rect2D getShape() const override { return rect2D(m_pos, m_size); };

	// It have a renderer to render a button style with a text on it.
	// It size can be defined but also auto sizing to the text size. 
	// It have a callback on it. It should be a callback that can be reached by world. 
	// So when I click on my window, I send to world the position, world will seek if there is an item on the click pos
	// If there is an item it will call its onclick function
	// If world have no callback, then a normal click will be performed. Can be linked to a callback also.

private:
	std::string m_label;
	Color m_color;
	Font* m_font;
	Color m_fontColor;
	std::string m_callbackID;
	std::string m_callbackReleaseID;
	buttonCallback m_callback;
	buttonCallback m_releaseCallback;
	
};

