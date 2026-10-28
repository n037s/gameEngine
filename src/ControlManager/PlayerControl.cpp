#include "PlayerControl.h"
#include "Player.h"

#include <vector>

const float move_sensibility = 1.3f;

const std::vector<timeoutItem> playerControlTimeouts = {
    timeoutItem(MOVE_RIGHT, 20),
    timeoutItem(MOVE_LEFT, 20),
    timeoutItem(MOVE_UP, 20),
    timeoutItem(MOVE_DOWN, 20)
};

bool PlayerControl::moveUp()
{
    if (checkTimeout(playerControlTimeouts.at(MOVE_UP)))
	{
		const point2D dx(0, -move_sensibility);
        Player::getInstance()->setAnimation("FarmRPG/Character/Idle_back");
		Player::getInstance()->movePos(dx);
        return true;
	}
    return false;
}

bool PlayerControl::moveDown()
{
    if (checkTimeout(playerControlTimeouts.at(MOVE_DOWN)))
    {
        const point2D dx(0, move_sensibility);
        Player::getInstance()->setAnimation("FarmRPG/Character/Idle_front");
        Player::getInstance()->movePos(dx);
        return true;
    }
    return false;
}

bool PlayerControl::moveLeft()
{
    if (checkTimeout(playerControlTimeouts.at(MOVE_LEFT)))
    {
        const point2D dx(-move_sensibility, 0);
        Player::getInstance()->setAnimation("FarmRPG/Character/Idle_left");
        Player::getInstance()->movePos(dx);
        return true;
    }
    return false;
}

bool PlayerControl::moveRight()
{
    if (checkTimeout(playerControlTimeouts.at(MOVE_RIGHT)))
    {
        const point2D dx(move_sensibility, 0);
        Player::getInstance()->setAnimation("FarmRPG/Character/Idle_right");
        Player::getInstance()->movePos(dx);
        return true;
    }
    return false;
}