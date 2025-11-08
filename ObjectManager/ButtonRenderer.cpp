#include "ButtonRenderer.h"

#include <iostream>


ButtonRenderer::ButtonRenderer(const SDL_FPoint pos, SDL_Renderer* renderer)
{
	// Here we define the button, its size and its look
}

ButtonRenderer::~ButtonRenderer()
{
	// Destroy the texture
}

bool ButtonRenderer::render(SDL_FRect& renderingRect)
{
    bool success = SDL_RenderTexture(m_renderer, m_texture, NULL, &renderingRect);
    if (!success)
    {
        std::cout << "[Error] rendering - " << SDL_GetError() << std::endl;
    }
    return success;
}