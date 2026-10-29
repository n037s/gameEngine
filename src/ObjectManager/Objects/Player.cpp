#include "Player.h"
#include "Camera.h"

#include <cmath>
#include <map>

using namespace PlayerMovement;

constexpr float playerSpeed = 2.3f;
constexpr float allowedDistCameraToPlayer = 30.0f;
constexpr uint16_t changeOrientationIterationThreshold = 3;

const std::map<int, std::string> movementToAsset = {
    { IDLE_RIGHT, "FarmRPG/Character/Idle_right"},
    { MOVE_RIGHT, "FarmRPG/Character/Walk_right"},
    { IDLE_LEFT, "FarmRPG/Character/Idle_left"},
    { MOVE_LEFT, "FarmRPG/Character/Walk_left"},
    { IDLE_UP, "FarmRPG/Character/Idle_back"},
    { MOVE_UP, "FarmRPG/Character/Walk_back"},
    { IDLE_DOWN, "FarmRPG/Character/Idle_front"},
    { MOVE_DOWN, "FarmRPG/Character/Walk_front"},
};

Player::Player(PlayerSingletonToken) : AnimatedSprite(point2D(0, 0), true, 250)
{
    setMovementOrientation(MOVE_DOWN, true);
}

Player::Player(PlayerSingletonToken, const ObjectMemberHolder& members) : AnimatedSprite(members)
{
    setMovementOrientation(MOVE_DOWN, true);
}

void Player::update()
{
    updateAnimation();

    // TODO : This part should be performed only at frame rate
    if (m_plannedMovement != vector2D(0, 0))
    {
        // A movement is planned, let's do it. 
        auto playerOrientation = computeOrientation();
        if (playerOrientation != m_lastMovement)
        {
            if (m_changeOrientationCount++; m_changeOrientationCount > changeOrientationIterationThreshold)
            {
                setMovementOrientation(playerOrientation, false);
            }
        }
        else
        {
            m_changeOrientationCount = 0;
        }

        auto mov = m_plannedMovement.normalized() * playerSpeed;
        setPos(m_pos + Camera::getInstance()->windowToWorldDelta(mov));
    }
}

void Player::move(const vector2D& dx)
{
    m_plannedMovement += dx;
}

void Player::event_stop_moving()
{
    setMovementOrientation(m_lastMovement, true);
}

void Player::setPos(const point2D& position)
{
    m_pos = position;
    m_plannedMovement = vector2D(0, 0);
    updateCameraPos();
}

const PlayerMovement::Direction Player::computeOrientation()
{
    PlayerMovement::Direction playerDirection{IDLE};


    // Decide the orientation of the player 
    if (std::abs(m_plannedMovement.x) >= std::abs(m_plannedMovement.y))
    {
        // Movement is more about x axis
        playerDirection = m_plannedMovement.x > 0 ? MOVE_RIGHT : MOVE_LEFT;
    }
    else
    {
        // Movement is more about y axis
        playerDirection = m_plannedMovement.y > 0 ? MOVE_DOWN : MOVE_UP;
    }
    return playerDirection;
}

void Player::setMovementOrientation(const PlayerMovement::Direction& movementOrientation, bool idle)
{
    m_lastMovement = movementOrientation;
    m_changeOrientationCount = 0;
    
    setAnimation(movementToAsset.at(movementOrientation + (idle ? 1 : 0)));
    if (idle) m_lastMovement = IDLE;
}

void Player::updateCameraPos()
{
    auto cameraPos = Camera::getInstance()->getPos();

    if (const auto distance = (getPos() - cameraPos).length(); distance >= allowedDistCameraToPlayer )
    {
        // compute the vector to move and update camera position
        vector2D moveVec = (getPos() - cameraPos).normalized() * (distance - allowedDistCameraToPlayer);
        Camera::getInstance()->movePos(moveVec);
    }
}