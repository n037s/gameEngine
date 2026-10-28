#include "ImageRenderer.h"

#include <iostream>

int main()
{
    if (!SDL_Init(0))
        return 1;

    SDL_Surface* target = SDL_CreateSurface(64, 64, SDL_PIXELFORMAT_RGBA8888);
    SDL_Renderer* renderer = target ? SDL_CreateSoftwareRenderer(target) : nullptr;
    if (!renderer)
    {
        std::cerr << SDL_GetError() << std::endl;
        SDL_DestroySurface(target);
        SDL_Quit();
        return 1;
    }

    bool success = false;
    {
        Image first(SDL_CreateSurface(16, 16, SDL_PIXELFORMAT_RGBA8888));
        Image second(SDL_CreateSurface(32, 32, SDL_PIXELFORMAT_RGBA8888));
        ImageRenderer imageRenderer(renderer, &first);
        SDL_Texture* firstTexture = imageRenderer.getTexture();
        imageRenderer.setAsset(&second);
        SDL_Texture* secondTexture = imageRenderer.getTexture();
        imageRenderer.setAsset(&first);
        success = firstTexture && secondTexture && firstTexture != secondTexture
            && imageRenderer.getTexture() == firstTexture && imageRenderer.isGenerated()
            && imageRenderer.render(SDL_FRect{0, 0, 16, 16});
        imageRenderer.setAsset(&second);
        success = success && imageRenderer.getTexture() == secondTexture;
        imageRenderer.setAsset(nullptr);
        success = success && !imageRenderer.isGenerated() && !imageRenderer.getTexture();
        imageRenderer.setAsset(&first);
        success = success && imageRenderer.isGenerated() && imageRenderer.getTexture() == firstTexture;
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroySurface(target);
    SDL_Quit();
    if (!success)
        std::cerr << "Animation frames must reuse their textures and recover from a missing asset." << std::endl;
    return success ? 0 : 1;
}