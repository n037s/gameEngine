#include "Camera.h"
#include <iostream>

const float z = 2;

Camera* Camera::m_instance = nullptr;

rect2D Camera::getRenderingRect()
{
	size2D renderingSize = windowSize * z;
	return rect2D(m_pos - renderingSize/2, renderingSize);
}

rect2D Camera::WorldToWindow(const rect2D& objShape)
{
	point2D pos = objShape.position - m_pos + windowSize * z / 2;
	size2D size = objShape.size / z;
	return rect2D(pos, size);
}

rect2D Camera::WindowToWorld(const rect2D& windowCoords)
{
	point2D pos = windowCoords.position + m_pos - windowSize * z / 2;
	size2D size = windowCoords.size * z;
	return rect2D(pos, size);
}

void Camera::movePos(point2D dpos)
{
    m_pos = m_pos + dpos;
}