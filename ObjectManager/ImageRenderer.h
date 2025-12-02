#pragma once
#include "BaseRenderer.h"
#include <string>
#include "Image.h"

class ImageRenderer :
    public BaseRenderer
{
public:
    ImageRenderer(SDL_Renderer* renderer, const Image* imageAsset);
    ~ImageRenderer();

    void setAsset(const Image* imageAsset);

private:
    void createTexture();

    const Image* m_image{ nullptr };
};

