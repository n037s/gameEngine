#include "rect.h"


const bool rect::isCollide(const rect& other) const
{
    if (m_position.x + m_size.w <= other.m_position.x ||
        other.m_position.x + other.m_size.w <= m_position.x ||
        m_position.y + m_size.h <= other.m_position.y ||
        other.m_position.y + other.m_size.h <= m_position.y ||
        depth + m_size.h <= other.depth ||
        other.depth + other.m_size.h <= depth)
        return false;
    return true;
}