#pragma once
#include "BaseRenderer.h"
#include <string>
#include "Geometry.h"

class ImageRenderer :
    public BaseRenderer
{
public:
    ImageRenderer(const SDL_FPoint pos, const std::string imagePath, SDL_Renderer* renderer);
    ~ImageRenderer();

    size2D getImageSize();
    bool render(SDL_FRect& renderingRect) override;

private:

    SDL_Surface* m_imageSurface{ nullptr };
};

