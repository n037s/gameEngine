#pragma once

#include "point2D.h"

class Mouse
{
public :
    static Mouse* getInstance();

    const point2D getPos() const;


private:
    Mouse() = default;
    ~Mouse() = default;

    static Mouse* m_instance;
};