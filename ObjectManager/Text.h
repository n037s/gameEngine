#pragma once
#include "Object.h"
#include "SDL_ttf.h"

#include <string>

class Text :
    public Object
{
public:
    Text(point2D pos, std::string text);


private:
    std::string m_text;
};

