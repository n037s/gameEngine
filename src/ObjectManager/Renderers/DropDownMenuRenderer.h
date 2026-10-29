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
    void setSize(size2D size) { m_size = size; }
    void addItem(ButtonRenderer* object, point2D position);
    void update();

private:
    size2D m_size;
    size2D m_itemSize;
    SDL_Color m_backgroundColor;
    std::list<std::pair<ButtonRenderer*, point2D>> m_objects;
};

