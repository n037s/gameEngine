#pragma once
#include "Object.h"

#include "Font.h"

#include <functional>
#include <string>

using buttonFunction = std::function<bool(point2D pos)>;

class Button : public Object
{
public:
	Button(rect2D shape, std::string text, SDL_Color& color, Font* font, SDL_Color& fontColor, 
		buttonFunction callback = nullptr, buttonFunction releaseCallback = nullptr);

	bool createRenderer(SDL_Renderer* renderer) override;
	void hoover() override;
	void leftFocus() override;
	bool leftClick(point2D pos) override;
	void releaseLeftClick(point2D pos) override;

	const rect2D getShape() const override { return rect2D(m_pos - m_size / 2, m_size); };

	// It have a renderer to render a button style with a text on it.
	// It size can be defined but also auto sizing to the text size. 
	// It have a callback on it. It should be a callback that can be reached by world. 
	// So when I click on my window, I send to world the position, world will seek if there is an item on the click pos
	// If there is an item it will call its onclick function
	// If world have no callback, then a normal click will be performed. Can be linked to a callback also.

private:
	std::string m_label;
	size2D m_size;
	SDL_Color m_color;
	Font* m_font;
	SDL_Color m_fontColor;
	buttonFunction m_callback;
	buttonFunction m_releaseCallback;
	
};

