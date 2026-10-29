#pragma once
#include "Text.h"

#include <chrono>

// This object will updates its text value with fps value
class FrameRateText : public Text
{
public:
    FrameRateText(const ObjectMemberHolder& members);
	FrameRateText(rect2D shape, const std::string& font, float fontSize, Color color = Color(0, 0, 0, 255));

    bool createRenderer(SDL_Renderer* renderer) override;
    void update() override;

    std::string getTypeName() const override { return "FrameRateText"; }
    ObjectMemberHolder serialize() const override;

    bool setColor(SDL_Color color);
    bool setFont(const std::string& font);
    void setPoliceSize(float size);
protected:
    std::chrono::steady_clock::time_point m_lastTime;
    int m_frameUpdated{ 0 };
    int m_displayRateTime_ms{ 500 };
};

