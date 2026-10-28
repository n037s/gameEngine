#include "PlayerControl.h"
#include "Player.h"

#include <vector>

const float move_sensibility = 2.3f;

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
        if (m_lastMoveId != MOVE_UP)
        {
            m_lastMoveId = MOVE_UP;
            Player::getInstance()->setAnimation("FarmRPG/Character/Walk_back");
        }
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
        if (m_lastMoveId != MOVE_DOWN)
        {
            m_lastMoveId = MOVE_DOWN;
            Player::getInstance()->setAnimation("FarmRPG/Character/Walk_front");
        }
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
        if (m_lastMoveId != MOVE_LEFT)
        {
            m_lastMoveId = MOVE_LEFT;
            std::cout << "Move left animation set" << std::endl;
            Player::getInstance()->setAnimation("FarmRPG/Character/Walk_left");
        }
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
        if (m_lastMoveId != MOVE_RIGHT)
        {
            m_lastMoveId = MOVE_RIGHT;
            Player::getInstance()->setAnimation("FarmRPG/Character/Walk_right");
        }
        Player::getInstance()->movePos(dx);
        return true;
    }
    return false;
}

bool PlayerControl::stopMoving()
{
    switch (m_lastMoveId)
    {
        case MOVE_UP:
            Player::getInstance()->setAnimation("FarmRPG/Character/Idle_back");
            break;
        case MOVE_DOWN:
            Player::getInstance()->setAnimation("FarmRPG/Character/Idle_front");
            break;
        case MOVE_LEFT:
            Player::getInstance()->setAnimation("FarmRPG/Character/Idle_left");
            break;
        case MOVE_RIGHT:
            Player::getInstance()->setAnimation("FarmRPG/Character/Idle_right");
            break;
    }
    return true;
}