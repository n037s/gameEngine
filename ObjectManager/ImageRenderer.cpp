#include "ImageRenderer.h"
#include <iostream>

ImageRenderer::ImageRenderer(const SDL_FPoint pos, const std::string imagePath, SDL_Renderer* renderer)
{
    m_renderer = renderer;
    m_imageSurface = SDL_LoadBMP(imagePath.c_str());
    if (!m_imageSurface) {
        std::cout << "Failed to load image "
            << imagePath << ":\n  " << SDL_GetError() << '\n';
    }
    else
    {
        m_isGenerated = true;
    }

    m_texture = SDL_CreateTextureFromSurface(m_renderer, m_imageSurface);
    if (m_texture == nullptr) {
        std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
    }

    std::cout << "renderer is generated : " << m_isGenerated << std::endl;
}

ImageRenderer::~ImageRenderer() {
    if (m_texture) {
        SDL_DestroyTexture(m_texture);
    }
    if (m_imageSurface) {
        SDL_DestroySurface(m_imageSurface);
    }
}

size2D ImageRenderer::getImageSize()
{
    return size2D(m_imageSurface->w, m_imageSurface->h);
}

bool ImageRenderer::render(SDL_FRect& renderingRect)
{
    bool success = SDL_RenderTexture(m_renderer, m_texture, NULL, &renderingRect);
    if (!success)
    {
        std::cout << "[Error] rendering - " << SDL_GetError() << std::endl;
    }
    return success;
}