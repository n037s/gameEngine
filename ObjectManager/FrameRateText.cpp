#include "FrameRateText.h"

#include "TextRenderer.h"

#include <iostream>

FrameRateText::FrameRateText(rect2D shape, std::string font, float fontSize, SDL_Color color) :
	Text(shape, "fps", font, fontSize, color)
{
}

bool FrameRateText::createRenderer(SDL_Renderer* renderer)
{
	m_renderer = new TextRenderer(renderer, m_text, m_size, new Font(m_font, m_fontSize), m_color);
	m_size = m_renderer->getSize();

	return m_renderer->isGenerated();
}

void FrameRateText::update()
{
	// we compute the fps rate
	m_frameUpdated++;
	auto currentTime = std::chrono::high_resolution_clock::now();
	auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - m_lastTime);

	if (duration_ms.count() > m_displayRateTime_ms)
	{
		float displayUpdatingRatio = 1000.0 / m_displayRateTime_ms;
		std::string fps = std::to_string((int)(displayUpdatingRatio * m_frameUpdated)) + "fps";
		static_cast<TextRenderer*>(m_renderer)->setText(fps);

		m_frameUpdated = 0;
		m_lastTime = currentTime;
	}
}

bool FrameRateText::setColor(SDL_Color color)
{
	bool success = false;
	TextRenderer* textRend = static_cast<TextRenderer*>(m_renderer);
	if (textRend)
	{
		textRend->setColor(color);
		success = textRend->isGenerated();
	}
	return success;
}

bool FrameRateText::setFont(std::string font)
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

void FrameRateText::setPoliceSize(float size)
{
	TextRenderer* textRend = static_cast<TextRenderer*>(m_renderer);
	if (textRend)
	{
		textRend->setPoliceSize(size);
	}
}
