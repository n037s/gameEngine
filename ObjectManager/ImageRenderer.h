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

    void setAsset(Image* imageAsset);

private:
    void createTexture();

    Image* m_image{ nullptr };
};

