#pragma once
#include "BaseRenderer.h"
#include "Geometry.h"
#include "Font.h"

class ButtonRenderer :
    public BaseRenderer
{
public:
    ButtonRenderer(SDL_Renderer* renderer, size2D size, SDL_Color color, const std::string& label, FontPtr font, SDL_Color fontColor);
    ~ButtonRenderer();

    void generateTexture();

    void setColor(SDL_Color color);

private:
    size2D m_size;
    SDL_Color m_color;
    std::string m_label;
    FontPtr m_font;
    SDL_Color m_fontColor;
};

