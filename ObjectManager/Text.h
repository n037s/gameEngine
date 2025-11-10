#pragma once
#include "Object.h"

#include <string>

class Text :
    public Object
{
public:
    Text(rect2D shape, std::string text, std::string font, float fontSize, SDL_Color color = {0, 0, 0, 1});

    bool createRenderer(SDL_Renderer* renderer) override;

    bool setText(std::string text);
    bool setColor(SDL_Color color);
    bool setFont(std::string font);
    void setPoliceSize(float size);

protected:
    std::string m_text;
    std::string m_font;
    float m_fontSize;
    SDL_Color m_color;
};

