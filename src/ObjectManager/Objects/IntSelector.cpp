#include "IntSelector.h"

#include "ButtonRenderer.h"
#include "DropDownMenuRenderer.h"

#include <algorithm>
#include <charconv>
#include <cmath>
#include <limits>

IntSelector::IntSelector()
	: IntSelector(rect2D(0, 0, 150, 30), 0, Color(180, 180, 180, 255),
		std::make_shared<Font>("arial", 20.0f), Color(0, 0, 0, 255))
{
}

IntSelector::IntSelector(rect2D shape, int value, Color color, FontPtr font, Color fontColor)
	: Object(shape.position, shape.size), m_value(value), m_color(color), m_font(std::move(font)), m_fontColor(fontColor)
{
}

IntSelector::IntSelector(const ObjectMemberHolder& members)
	: Object(members), m_value(members.getMember<int>("intSelectorValue")),
	  m_color(members.getMember<Color>("intSelectorColor")),
	  m_font(members.getMember<FontPtr>("intSelectorFont")),
	  m_fontColor(members.getMember<Color>("intSelectorFontColor"))
{
}

IntSelector::~IntSelector() = default;

bool IntSelector::createRenderer(SDL_Renderer* renderer)
{
	if (!renderer || !m_font || !m_font->isGenerated() || m_size.w < 3 || m_size.h < 1)
		return false;

	delete m_renderer;
	m_renderer = nullptr;
	m_sdlRenderer = renderer;
	const float sideWidth = std::min(m_size.h, m_size.w / 3.0f);
	const size2D sideSize(sideWidth, m_size.h);
	const size2D valueSize(m_size.w - 2 * sideWidth, m_size.h);
	m_buttons[0] = std::make_unique<ButtonRenderer>(renderer, sideSize, m_color.toSDL(), "-", m_font, m_fontColor.toSDL());
	const auto label = m_labelPrefix + (m_editing ? m_editText.substr(0, m_cursor) + "|" + m_editText.substr(m_cursor) : std::to_string(m_value));
	m_buttons[1] = std::make_unique<ButtonRenderer>(renderer, valueSize, m_color.toSDL(), label, valueFont(label), m_fontColor.toSDL());
	m_selectionHighlighted = false;
	m_buttons[2] = std::make_unique<ButtonRenderer>(renderer, sideSize, m_color.toSDL(), "+", m_font, m_fontColor.toSDL());
	auto menu = new DropDownMenuRenderer(renderer, m_size, sideSize, m_color.toSDL());
	m_renderer = menu;
	menu->addItem(m_buttons[0].get(), point2D(0, 0));
	menu->addItem(m_buttons[1].get(), point2D(sideWidth, 0));
	menu->addItem(m_buttons[2].get(), point2D(m_size.w - sideWidth, 0));
	menu->update();
	if (m_editing && m_selectAll)
		updateValueLabel();
	return menu->getTexture() != nullptr;
}

ObjectMemberHolder IntSelector::serialize() const
{
	ObjectMemberHolder members = Object::serialize();
	members.addMember("intSelectorValue", m_value);
	members.addMember("intSelectorColor", m_color);
	members.addMember("intSelectorFont", m_font);
	members.addMember("intSelectorFontColor", m_fontColor);
	return members;
}

void IntSelector::setValue(int value)
{
	if (m_value == value)
		return;
	m_value = value;
	if (m_editing)
	{
		m_editText = std::to_string(m_value);
		m_cursor = m_editText.size();
		m_selectAll = true;
	}
	updateValueLabel();
	if (m_onChange)
		m_onChange(m_value);
}

void IntSelector::setSize(const size2D& size)
{
	if (m_size.w == size.w && m_size.h == size.h)
		return;
	m_size = size;
	if (m_sdlRenderer)
		createRenderer(m_sdlRenderer);
}

void IntSelector::setLabelPrefix(const std::string& prefix)
{
	if (m_labelPrefix == prefix)
		return;
	m_labelPrefix = prefix;
	updateValueLabel();
}

int IntSelector::areaAt(point2D pos) const
{
	if (!getShape().isInside(pos))
		return 0;
	const float sideWidth = std::min(m_size.h, m_size.w / 3.0f);
	if (pos.x < m_pos.x + sideWidth)
		return -1;
	if (pos.x >= m_pos.x + m_size.w - sideWidth)
		return 1;
	return 2;
}

bool IntSelector::leftClick(point2D pos)
{
	if (m_editing)
		finishTextInput(true);
	m_pressedArea = areaAt(pos);
	return getShape().isInside(pos);
}

bool IntSelector::releaseLeftClick(point2D pos)
{
	const int pressedArea = m_pressedArea;
	m_pressedArea = 0;
	if (pressedArea == 0 || pressedArea != areaAt(pos))
		return false;
	if (pressedArea == 2)
	{
		m_editing = true;
		m_editText = std::to_string(m_value);
		m_cursor = m_editText.size();
		m_selectAll = true;
		updateValueLabel();
		return true;
	}
	if ((pressedArea < 0 && m_value == std::numeric_limits<int>::min())
		|| (pressedArea > 0 && m_value == std::numeric_limits<int>::max()))
		return false;
	setValue(m_value + pressedArea);
	return true;
}

void IntSelector::handleTextInput(const std::string& text)
{
	if (!m_editing || text.empty())
		return;
	std::string candidate = m_selectAll ? std::string() : m_editText;
	const size_t cursor = m_selectAll ? 0 : m_cursor;
	candidate.insert(cursor, text);
	if (candidate.size() > 11)
		return;
	for (size_t index = 0; index < candidate.size(); ++index)
		if ((candidate[index] < '0' || candidate[index] > '9') && !(index == 0 && candidate[index] == '-'))
			return;
	m_editText = std::move(candidate);
	m_cursor = cursor + text.size();
	m_selectAll = false;
	updateValueLabel();
}

void IntSelector::handleTextKey(SDL_Keycode key, SDL_Keymod modifiers)
{
	if (!m_editing)
		return;
	if (key == SDLK_RETURN || key == SDLK_KP_ENTER)
	{
		finishTextInput(true);
		return;
	}
	if (key == SDLK_ESCAPE)
	{
		finishTextInput(false);
		return;
	}
	if ((modifiers & SDL_KMOD_CTRL) && key == SDLK_A)
	{
		m_selectAll = true;
		m_cursor = m_editText.size();
	}
	else if ((modifiers & SDL_KMOD_CTRL) && key == SDLK_V)
	{
		char* clipboard = SDL_GetClipboardText();
		if (clipboard)
		{
			handleTextInput(clipboard);
			SDL_free(clipboard);
		}
		return;
	}
	else if (key == SDLK_BACKSPACE || key == SDLK_DELETE)
	{
		if (m_selectAll)
		{
			m_editText.clear();
			m_cursor = 0;
		}
		else if (key == SDLK_BACKSPACE && m_cursor > 0)
			m_editText.erase(--m_cursor, 1);
		else if (key == SDLK_DELETE && m_cursor < m_editText.size())
			m_editText.erase(m_cursor, 1);
		m_selectAll = false;
	}
	else if (key == SDLK_LEFT || key == SDLK_RIGHT || key == SDLK_HOME || key == SDLK_END)
	{
		if (key == SDLK_HOME || (m_selectAll && key == SDLK_LEFT))
			m_cursor = 0;
		else if (key == SDLK_END || (m_selectAll && key == SDLK_RIGHT))
			m_cursor = m_editText.size();
		else if (key == SDLK_LEFT && m_cursor > 0)
			--m_cursor;
		else if (key == SDLK_RIGHT && m_cursor < m_editText.size())
			++m_cursor;
		m_selectAll = false;
	}
	else
		return;
	updateValueLabel();
}

void IntSelector::finishTextInput(bool commit)
{
	if (!m_editing)
		return;
	int value = 0;
	const auto result = std::from_chars(m_editText.data(), m_editText.data() + m_editText.size(), value);
	const bool valid = result.ec == std::errc() && result.ptr == m_editText.data() + m_editText.size();
	m_editing = false;
	m_selectAll = false;
	if (commit && valid && value != m_value)
		setValue(value);
	else
		updateValueLabel();
	if (commit && valid && m_onCommit)
		m_onCommit(value);
}

void IntSelector::updateValueLabel()
{
	if (!m_buttons[1])
		return;
	const bool highlight = m_editing && m_selectAll;
	if (highlight != m_selectionHighlighted)
	{
		SDL_Color color = m_color.toSDL();
		if (highlight)
		{
			const int change = (static_cast<int>(color.r) + color.g + color.b) / 3 > 180 ? -35 : 35;
			color.r = static_cast<Uint8>(std::clamp(static_cast<int>(color.r) + change, 0, 255));
			color.g = static_cast<Uint8>(std::clamp(static_cast<int>(color.g) + change, 0, 255));
			color.b = static_cast<Uint8>(std::clamp(static_cast<int>(color.b) + change, 0, 255));
		}
		m_buttons[1]->setColor(color);
		m_selectionHighlighted = highlight;
	}
	const auto label = m_labelPrefix + (m_editing ? m_editText.substr(0, m_cursor) + "|" + m_editText.substr(m_cursor) : std::to_string(m_value));
	m_buttons[1]->setLabel(label, valueFont(label));
	updateRenderer();
}

void IntSelector::updateRenderer()
{
	if (m_renderer)
		static_cast<DropDownMenuRenderer*>(m_renderer)->update();
}

FontPtr IntSelector::valueFont(const std::string& label) const
{
	const float sideWidth = std::min(m_size.h, m_size.w / 3.0f);
	const int availableWidth = static_cast<int>(m_size.w - 2 * sideWidth);
	int textWidth = 0;
	int textHeight = 0;
	if (!TTF_GetStringSize(m_font->toSDL(), label.c_str(), 0, &textWidth, &textHeight)
		|| textWidth <= availableWidth)
		return m_font;

	float fontSize = m_font->getPolicySize();
	FontPtr fittedFont;
	do
	{
		fontSize = std::max(1.0f, std::min(fontSize - 1.0f,
			std::floor(fontSize * std::max(1, availableWidth - 2) / textWidth)));
		fittedFont = std::make_shared<Font>(m_font->getFontID(), fontSize, m_font->getTextAlignement());
		if (!fittedFont->isGenerated()
			|| !TTF_GetStringSize(fittedFont->toSDL(), label.c_str(), 0, &textWidth, &textHeight))
			return m_font;
	} while (textWidth > availableWidth && fontSize > 1.0f);
	return fittedFont;
}