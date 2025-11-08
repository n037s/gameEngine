#include "CameraControl.h"

const int move_sensibility = 10;

bool CameraControl::moveUp()
{
	point2D dx(0, -move_sensibility);
	Camera::getInstance()->movePos(dx);
	return true;
}

bool CameraControl::moveDown()
{
	point2D dx(0, move_sensibility);
	Camera::getInstance()->movePos(dx);
	return true;
}

bool CameraControl::moveLeft()
{
	point2D dx(-move_sensibility, 0);
	Camera::getInstance()->movePos(dx);
	return true;
}

bool CameraControl::moveRight()
{
	point2D dx(move_sensibility, 0);
	Camera::getInstance()->movePos(dx);
	return true;
}