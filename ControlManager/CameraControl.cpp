#include "CameraControl.h"

const int move_sensibility = 10;

bool CameraControl::moveUp()
{
	const point2D dx(0, -move_sensibility);
	Camera::getInstance()->movePos(dx);
	return true;
}

bool CameraControl::moveDown()
{
	const point2D dx(0, move_sensibility);
	Camera::getInstance()->movePos(dx);
	return true;
}

bool CameraControl::moveLeft()
{
	const point2D dx(-move_sensibility, 0);
	Camera::getInstance()->movePos(dx);
	return true;
}

bool CameraControl::moveRight()
{
	const point2D dx(move_sensibility, 0);
	Camera::getInstance()->movePos(dx);
	return true;
}

bool CameraControl::moveZ(float dz)
{
	const float wheelRatio = -0.2f;
	Camera::getInstance()->moveZ(dz * wheelRatio);
	return true;
}