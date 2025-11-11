#include "ImageRenderer.h"
#include <iostream>

ImageRenderer::ImageRenderer(SDL_Renderer* renderer, Image* imageAsset)
{
    m_renderer = renderer;
    m_image = imageAsset;
    createTexture();
}

ImageRenderer::~ImageRenderer() {
    cleanRenderer();
}

void ImageRenderer::setAsset(Image* imageAsset)
{
    m_image = imageAsset;
    cleanRenderer();
    createTexture();
}


void ImageRenderer::createTexture()
{
    if (m_image)
    {
        m_surface = m_image->getSurface();
        if (m_surface != nullptr)
        {
            m_texture = SDL_CreateTextureFromSurface(m_renderer, m_surface);
            if (!m_texture) {
                std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
            }
            else
            {
                m_isGenerated = true;
            }
        }
        else
        {
            std::cout << "WTF error surface pointer is null" << std::endl;
        }
    }
    else
    {
        std::cout << "Asset is not loaded" << std::endl;
    }
}