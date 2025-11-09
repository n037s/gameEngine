#pragma once
#include "BaseRenderer.h"

#include "Geometry.h"

#include "SDL_ttf.h"

class TextRenderer :
    public BaseRenderer
{
public:
    TextRenderer(const SDL_FPoint& pos, const std::string& text, const std::string font, const float fontSize, SDL_Renderer* renderer, SDL_Color& color);
    ~TextRenderer();

    void cleanTexture();
    void generateTexture();

    void setText(std::string& text);
    void setColor(SDL_Color& color);
    void setFont(std::string& font);
    void setPoliceSize(float size);

private:
    std::string m_text;
    TTF_Font* m_font;
    SDL_Color m_color;
    std::string m_policy{ "" };
    float m_policySize;
};

