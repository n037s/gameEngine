#include "ImageRenderer.h"
#include <iostream>

ImageRenderer::ImageRenderer(SDL_Renderer* renderer, const Image* imageAsset)
{
    m_renderer = renderer;
    m_image = imageAsset;
    createTexture();
}

ImageRenderer::~ImageRenderer() {
}

void ImageRenderer::setAsset(const Image* imageAsset)
{
    m_image = imageAsset;
    createTexture();
}

void ImageRenderer::resetAsset(const Image* imageAsset)
{
    m_textures.clear();
    setAsset(imageAsset);
}

void ImageRenderer::createTexture()
{
    m_isGenerated = false;
    m_texture = nullptr;
    m_surface = nullptr;
    if (m_image)
    {
        m_surface = m_image->getSurface();
        if (m_surface != nullptr)
        {
            const auto cached = m_textures.find(m_image);
            if (cached != m_textures.end())
                m_texture = cached->second.get();
            else
            {
                std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> texture(
                    SDL_CreateTextureFromSurface(m_renderer, m_surface), SDL_DestroyTexture);
                if (texture)
                    m_texture = m_textures.emplace(m_image, std::move(texture)).first->second.get();
            }
            if (!m_texture) {
                m_logger->error("SDL_CreateTextureFromSurface Error: " + std::string(SDL_GetError()));
            }
            else
            {
                m_isGenerated = true;
            }
        }
        else
        {
            m_logger->error("WTF error surface pointer is null");
        }
    }
    else
    {
        m_logger->error("Asset is not loaded");
    }
}