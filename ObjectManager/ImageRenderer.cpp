#include "ImageRenderer.h"
#include <iostream>

ImageRenderer::ImageRenderer(SDL_Renderer* renderer, const std::string imagePath)
{
    m_renderer = renderer;
    m_surface = SDL_LoadBMP(imagePath.c_str());
    if (!m_surface) {
        std::cout << "Failed to load image "
            << imagePath << ":\n  " << SDL_GetError() << '\n';
    }
    else
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
}

ImageRenderer::~ImageRenderer() {
    cleanSurfaceAndRenderer();
}