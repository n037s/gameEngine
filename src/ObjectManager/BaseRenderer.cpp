#include "BaseRenderer.h"

#include <iostream>

void BaseRenderer::cleanSurfaceAndRenderer()
{
    if (m_surface)
    {
        SDL_DestroySurface(m_surface);
    }
    if (m_texture)
    {
        SDL_DestroyTexture(m_texture);
    }
}

void BaseRenderer::cleanRenderer()
{
    if (m_texture)
    {
        SDL_DestroyTexture(m_texture);
    }
}

size2D BaseRenderer::getSize() const
{
    size2D result = size2D(0, 0);
    if (m_surface)
    {
        result = size2D(static_cast<float>(m_surface->w), static_cast<float>(m_surface->h));
    }
    return result;
}

bool BaseRenderer::render(const SDL_FRect& renderingRect)
{
    bool success = false;
    if (m_isGenerated)
    {
        success = SDL_RenderTexture(m_renderer, m_texture, NULL, &renderingRect);
        if (!success)
        {
            std::cout << "[Error] rendering - " << SDL_GetError() << std::endl;
        }
    }
    return success;
}

bool BaseRenderer::renderQuad(const std::array<point2D, 4>& corners)
{
    if (!m_isGenerated || !m_texture)
        return false;

    const SDL_FColor white{ 1.0f, 1.0f, 1.0f, 1.0f };
    const SDL_Vertex vertices[] = {
        { corners[0].toSDL(), white, { 0.0f, 0.0f } },
        { corners[1].toSDL(), white, { 1.0f, 0.0f } },
        { corners[2].toSDL(), white, { 1.0f, 1.0f } },
        { corners[3].toSDL(), white, { 0.0f, 1.0f } }
    };
    const int indices[] = { 0, 1, 2, 0, 2, 3 };
    const bool success = SDL_RenderGeometry(m_renderer, m_texture, vertices, 4, indices, 6);
    if (!success)
        std::cout << "[Error] rendering quad - " << SDL_GetError() << std::endl;
    return success;
}
