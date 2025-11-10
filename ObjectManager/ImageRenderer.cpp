#include "ImageRenderer.h"
#include <iostream>

ImageRenderer::ImageRenderer(SDL_Renderer* renderer, Image* imageAsset)
{
    m_renderer = renderer;
    m_image = imageAsset;
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
                std::cout << "renderer is generated : " << m_isGenerated << std::endl;
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

ImageRenderer::~ImageRenderer() {
    cleanRenderer();
}