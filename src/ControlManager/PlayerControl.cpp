#include "PlayerControl.h"
#include "Player.h"
#include "Camera.h"

#include <vector>

const float move_sensibility = 2.3f;

const std::vector<timeoutItem> playerControlTimeouts = {
    timeoutItem(IDLE, 1),
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
        if (m_lastMoveId == IDLE)
        {
            m_lastMoveId = MOVE_UP;
            Player::getInstance()->setAnimation("FarmRPG/Character/Walk_back");
        }
		Player::getInstance()->movePos(Camera::getInstance()->windowToWorldDelta(dx));
        updateCameraPos();
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
        Player::getInstance()->movePos(Camera::getInstance()->windowToWorldDelta(dx));
        updateCameraPos();
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
        Player::getInstance()->movePos(Camera::getInstance()->windowToWorldDelta(dx));
        updateCameraPos();
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
        Player::getInstance()->movePos(Camera::getInstance()->windowToWorldDelta(dx));
        updateCameraPos();
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
    m_lastMoveId = IDLE;
    return true;
}

bool PlayerControl::resetPos()
{
    Player::getInstance()->setPosition(point2D(0, 0));
    Camera::getInstance()->setPos(point2D(0, 0));
    Player::getInstance()->setAnimation("FarmRPG/Character/Idle_front");
    m_lastMoveId = IDLE;
    return true;
}

void PlayerControl::updateCameraPos()
{
    const float allowedDist = 30.0f;

    auto playerPos = Player::getInstance()->getPos();
    auto cameraPos = Camera::getInstance()->getPos();

    if (const auto distance = (playerPos - cameraPos).length(); distance >= allowedDist )
    {
        // compute the vector to move and update camera position
        vector2D moveVec = (playerPos - cameraPos).normalized() * (distance - allowedDist);
        Camera::getInstance()->movePos(moveVec);
    }
}