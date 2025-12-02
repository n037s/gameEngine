#pragma once
#include "Object.h"

#include "Color.h"

#include <string>

class Text :
    public Object
{
public:
    Text(const ObjectMemberHolder& members);
    Text(rect2D shape, const std::string& text, const std::string& font, float fontSize, Color color = Color(0, 0, 0, 255));

    bool createRenderer(SDL_Renderer* renderer) override;

    std::string getTypeName() const override { return "Text"; }
    ObjectMemberHolder serialize() const override;

    bool setText(const std::string& text);
    bool setColor(Color color);
    bool setFont(const std::string& font);
    void setPoliceSize(float size);

protected:
    std::string m_text;
    std::string m_font;
    float m_fontSize;
    Color m_color;
};

