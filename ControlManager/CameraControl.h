#pragma once
#include "Camera.h"

class CameraControl
{
private:

public:
	CameraControl() {}
	
	static bool moveUp();
	static bool moveDown();
	static bool moveLeft();
	static bool moveRight();
};