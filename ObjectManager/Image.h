#pragma once

#include "Asset.h"

#include "SDL3/SDL.h"

#include <iostream>



class Image :
    public Asset
{
public:
    Image(SDL_Surface* surface);
    ~Image() 
    {
            if (m_surface)
            {
                SDL_DestroySurface(m_surface);
            }
    }

    void setSurface(SDL_Surface* surface) { m_surface = surface; }
    SDL_Surface* getSurface() { return m_surface; }

    void setpath(std::string p) { filePath = p; }
    std::string getpath() { return filePath; }

private:
    std::string filePath;
    SDL_Surface* m_surface{ nullptr };
};

