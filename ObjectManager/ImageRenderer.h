#pragma once
#include "BaseRenderer.h"
#include <string>

class ImageRenderer :
    public BaseRenderer
{
public:
    ImageRenderer(SDL_Renderer* renderer, const std::string imagePath);
    ~ImageRenderer();

private:

};

