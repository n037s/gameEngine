#pragma once 

#include "Object.h"

#include <array>
#include <functional>
#include <memory>

class ButtonRenderer;

class IntSelector : public Object
{
public:
	IntSelector();
	IntSelector(rect2D shape, int value, Color color, FontPtr font, Color fontColor);
	IntSelector(const ObjectMemberHolder& members);
	~IntSelector() override;

	bool createRenderer(SDL_Renderer* renderer) override;
	std::string getTypeName() const override { return "IntSelector"; }
	ObjectMemberHolder serialize() const override;

	int getValue() const { return m_value; }
	void setValue(int value);
	void setSize(const size2D& size) override;
	void setLabelPrefix(const std::string& prefix);
	void setOnChange(std::function<void(int)> callback) { m_onChange = std::move(callback); }
	void setOnCommit(std::function<void(int)> callback) { m_onCommit = std::move(callback); }
	bool leftClick(point2D pos) override;
	bool releaseLeftClick(point2D pos) override;
	bool wantsTextInput() const override { return m_editing; }
	void handleTextInput(const std::string& text) override;
	void handleTextKey(SDL_Keycode key, SDL_Keymod modifiers) override;
	void finishTextInput(bool commit) override;

private:
	int areaAt(point2D pos) const;
	void updateRenderer();
	void updateValueLabel();
	FontPtr valueFont(const std::string& label) const;

	int m_value{ 0 };
	int m_pressedArea{ 0 };
	bool m_editing{ false };
	bool m_selectAll{ false };
	bool m_selectionHighlighted{ false };
	std::string m_editText;
	std::string m_labelPrefix;
	size_t m_cursor{ 0 };
	Color m_color;
	FontPtr m_font;
	Color m_fontColor;
	std::array<std::unique_ptr<ButtonRenderer>, 3> m_buttons;
	std::function<void(int)> m_onChange;
	std::function<void(int)> m_onCommit;
	SDL_Renderer* m_sdlRenderer{ nullptr };
};