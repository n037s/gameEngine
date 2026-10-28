#pragma once

#include "BaseController.h"

enum PlayerControlTimeout
{
    MOVE_RIGHT = 0,
    MOVE_LEFT,
    MOVE_UP,
    MOVE_DOWN
};

class PlayerControl : public BaseController
{
private:
	PlayerControl() : BaseController() {}

public:
	static PlayerControl& getInstance() {
		static PlayerControl instance;
		return instance;
	}
	
	bool moveUp();
	bool moveDown();
	bool moveLeft();
	bool moveRight();
	bool stopMoving();

	int m_lastMoveId{-1};
};