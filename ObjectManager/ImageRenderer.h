#pragma once
#include "BaseRenderer.h"
#include <string>
#include "Image.h"

class ImageRenderer :
    public BaseRenderer
{
public:
    ImageRenderer(SDL_Renderer* renderer, Image* imageAsset);
    ~ImageRenderer();

private:
    Image* m_image{ nullptr };
};

