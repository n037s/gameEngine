#include "Camera.h"

#include <iostream>
#include <algorithm>

Camera* Camera::m_instance = nullptr;

rect2D Camera::getRenderingRect()
{
	size2D renderingSize = windowSize * m_height;
	return rect2D(m_pos - renderingSize / 2, renderingSize);
}

rect2D Camera::WorldToWindow(const rect2D& objShape)
{
	point2D posInCamera = (objShape.position - m_pos) / m_height;
	point2D posInWindow = posInCamera + windowSize/2;

	size2D size = objShape.size / m_height;
	return rect2D(posInWindow, size);
}

rect2D Camera::WindowToWorld(const rect2D& windowCoords)
{
	point2D posInCamera = windowCoords.position - windowSize / 2;
	point2D posInWorld = posInCamera * m_height + m_pos;

	size2D size = windowCoords.size * m_height;
	return rect2D(posInWorld, size);
}

void Camera::movePos(point2D dpos)
{
    m_pos = m_pos + dpos;
}

void Camera::moveZ(float dz)
{
	const float cameraMinZ = 0.2;
	const float cameraMaxZ = 5;
	m_height = std::clamp(m_height + dz, cameraMinZ, cameraMaxZ);
}

float Camera::getRenderingScale()
{
	return 1 / m_height;
}