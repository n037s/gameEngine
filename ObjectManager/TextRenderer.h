#pragma once
#include "BaseRenderer.h"

#include "Geometry.h"
#include "Font.h"

#include "SDL_ttf.h"

class TextRenderer :
    public BaseRenderer
{
public:
    TextRenderer(SDL_Renderer* renderer, const std::string& text, size2D size, Font* font, SDL_Color& color);
    ~TextRenderer();

    void cleanTexture();
    void generateTexture();

    void setText(std::string& text);
    void setColor(SDL_Color& color);
    void setFont(std::string& font);
    void setPoliceSize(float size);

private:
    bool setupTextInSurface(SDL_Surface* textSurface);

    std::string m_text;
    size2D m_size;
    SDL_Color m_color;
    Font* m_font;
};

