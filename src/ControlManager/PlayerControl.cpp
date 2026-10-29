#include "PlayerControl.h"
#include "Player.h"

#include <vector>

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
		const vector2D dx(0, -1);
        Player::getInstance()->move(dx);
        return true;
	}
    return false;
}

bool PlayerControl::moveDown()
{
    if (checkTimeout(playerControlTimeouts.at(MOVE_DOWN)))
    {
        const vector2D dx(0, 1);
        Player::getInstance()->move(dx);
        return true;
    }
    return false;
}

bool PlayerControl::moveLeft()
{
    if (checkTimeout(playerControlTimeouts.at(MOVE_LEFT)))
    {
        const vector2D dx(-1, 0);
        Player::getInstance()->move(dx);
        return true;
    }
    return false;
}

bool PlayerControl::moveRight()
{
    if (checkTimeout(playerControlTimeouts.at(MOVE_RIGHT)))
    {
        const vector2D dx(1, 0);
        Player::getInstance()->move(dx);
        return true;
    }
    return false;
}

bool PlayerControl::resetPos()
{
    Player::getInstance()->setPos(point2D(0, 0));
    Player::getInstance()->setAnimation("FarmRPG/Character/Idle_front");
    return true;
}