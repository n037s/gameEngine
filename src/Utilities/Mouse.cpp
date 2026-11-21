#include "Mouse.h"
#include <SDL3/SDL.h>

Mouse* Mouse::m_instance = nullptr;

Mouse* Mouse::getInstance()
{
    if (m_instance == nullptr)
        m_instance = new Mouse();
    return m_instance;
}

const point2D Mouse::getPos() const
{
    float x, y;
    SDL_GetMouseState(&x, &y);
    return point2D(x, y);
}