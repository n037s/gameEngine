#pragma once
#include "BaseRenderer.h"

#include "ButtonRenderer.h"

#include <list>

class DropDownMenuRenderer :
    public BaseRenderer
{
public:
    DropDownMenuRenderer(SDL_Renderer* renderer, size2D size, size2D itemSize, SDL_Color backgroundColor);
    ~DropDownMenuRenderer();

    void clearItems();
    void addItem(ButtonRenderer* object);
    void update();

private:
    size2D m_size;
    size2D m_itemSize;
    SDL_Color m_backgroundColor;
    std::list<ButtonRenderer*> m_objects;
};

