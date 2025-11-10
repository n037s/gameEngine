#pragma once
#include "Text.h"
#include <chrono>

// This object will updates its text value with fps value
class FrameRateText : public Text
{
public:
	FrameRateText(rect2D shape, std::string font, float fontSize, SDL_Color color = { 0, 0, 0, 1 });

    bool createRenderer(SDL_Renderer* renderer) override;
    void update() override;

    bool setColor(SDL_Color color);
    bool setFont(std::string font);
    void setPoliceSize(float size);
protected:
    std::chrono::steady_clock::time_point m_lastTime;
    int m_frameUpdated{ 0 };
    int m_displayRateTime_ms{ 500 };
};

