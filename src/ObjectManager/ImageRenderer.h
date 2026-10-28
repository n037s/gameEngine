#pragma once
#include "BaseRenderer.h"
#include <string>
#include <memory>
#include <unordered_map>
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
    std::unordered_map<const Image*, std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>> m_textures;
};

