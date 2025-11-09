#pragma once
#include "BaseRenderer.h"
#include <string>

class ImageRenderer :
    public BaseRenderer
{
public:
    ImageRenderer(const SDL_FPoint pos, const std::string imagePath, SDL_Renderer* renderer);
    ~ImageRenderer();

private:

};

