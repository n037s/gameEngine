#pragma once
#include "BaseRenderer.h"
#include "Geometry.h"

class ButtonRenderer :
    public BaseRenderer
{
public:
    ButtonRenderer(const SDL_FPoint pos, SDL_Renderer* renderer);
    ~ButtonRenderer();

    size2D getImageSize();
    bool render(SDL_FRect& renderingRect) override;

private:


};

