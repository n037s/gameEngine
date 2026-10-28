#pragma once
#include "Camera.h"
#include "BaseController.h"

enum CameraControlTimeout
{
    TOGGLE_VIEW_MODE = 0
};

class CameraControl : public BaseController
{
private:
	CameraControl() : BaseController() {}

public:
	static CameraControl& getInstance() {
		static CameraControl instance;
		return instance;
	}
	
	bool moveUp();
	bool moveDown();
	bool moveLeft();
	bool moveRight();
	bool moveZ(float dz);
	bool toggleViewMode();
};