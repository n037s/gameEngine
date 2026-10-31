#include "PlayerControl.h"
#include "Player.h"

bool PlayerControl::moveUp()
{
    Player::getInstance()->move(vector2D(0, -1));
    return true;
}

bool PlayerControl::moveDown()
{
    Player::getInstance()->move(vector2D(0, 1));
    return true;
}

bool PlayerControl::moveLeft()
{
    Player::getInstance()->move(vector2D(-1, 0));
    return true;
}

bool PlayerControl::moveRight()
{
    Player::getInstance()->move(vector2D(1, 0));
    return true;
}

bool PlayerControl::stopMoving()
{
    Player::getInstance()->event_stop_moving();
    return true;
}

bool PlayerControl::resetPos()
{
    Player::getInstance()->setPos(point2D(0, 0));
    Player::getInstance()->setAnimation("FarmRPG/Character/Idle_front");
    return true;
}