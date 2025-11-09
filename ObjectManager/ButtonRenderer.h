#pragma once
#include "BaseRenderer.h"
#include "Geometry.h"

class ButtonRenderer :
    public BaseRenderer
{
public:
    ButtonRenderer(const SDL_FPoint pos, SDL_Renderer* renderer);
    ~ButtonRenderer();

    void generateTexture();

private:


};

