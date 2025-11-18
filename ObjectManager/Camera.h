#pragma once

#include "Geometry.h"


class Camera
{
public:
	static Camera* getInstance() {
		if (!m_instance)
			m_instance = new Camera(point2D(0,0), size2D(0,0));
		return m_instance;
	}

	point2D getPos() { return m_pos; }
	size2D getWindowSize() { return windowSize; }
	void setPos(point2D pos) { m_pos = pos; }
	void setWindowSize(size2D size) { windowSize = size; }
	void setCameraZLimits(float zMin, float zMax) { m_minCameraZ = zMin; m_maxCameraZ = zMax; }

	void movePos(point2D dpos);
	void moveZ(float z);
	float getZ() { return m_height; }
	float getRenderingScale();

	rect2D getRenderingRect();
	rect2D WorldToWindow(const rect2D& objShape);
	rect2D WindowToWorld(const rect2D& windowCoords);

private:
	static Camera* m_instance;
	Camera(point2D pos, size2D windowSize) : m_pos{ pos }, windowSize(windowSize) {};

	point2D m_pos;
	float m_height{ 1 };
	float m_minCameraZ{ 0 };
	float m_maxCameraZ{ 5 };
	size2D windowSize;
};
