#include "BaseRenderer.h"

#include <iostream>

size2D BaseRenderer::getSize()
{
    size2D result = size2D(0, 0);
    if (m_surface)
    {
        result = size2D(m_surface->w, m_surface->h);
    }
    return result;
}

bool BaseRenderer::render(SDL_FRect& renderingRect)
{
    bool success = false;
    if (m_isGenerated)
    {
        bool success = SDL_RenderTexture(m_renderer, m_texture, NULL, &renderingRect);
        if (!success)
        {
            std::cout << "[Error] rendering - " << SDL_GetError() << std::endl;
        }
    }
    return success;
}
