#include "Player.h"
#include "Camera.h"

#include <cmath>
#include <map>

using namespace PlayerMovement;

constexpr float playerSpeed = 2.3f;
constexpr float allowedDistCameraToPlayer = 30.0f;

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
    std::cout << "Player created without members" << std::endl;
    setMovementOrientation(IDLE_DOWN, true);
}

Player::Player(PlayerSingletonToken, const ObjectMemberHolder& members) : AnimatedSprite(members)
{
    std::cout << "Player created from members" << std::endl;
    setMovementOrientation(IDLE_DOWN, true);
}

void Player::update()
{
    if (m_plannedMovement != vector2D(0, 0))
    {
        std::cout << "Player::update() - move for : " << m_plannedMovement.toString() << std::endl;
        // A movement is planned, let's do it. 
        // TODO : correct the movement speed according to the FPS
        auto mov = m_plannedMovement.normalized() * playerSpeed;
        std::cout << "Movement of Player : " << mov.toString() << std::endl;
        std::cout << "So position goes from : " << getPos().toString() << " - to " << (getPos() + Camera::getInstance()->windowToWorldDelta(mov)).toString() << std::endl;
        setPos(getPos() + Camera::getInstance()->windowToWorldDelta(mov));
    
        auto playerOrientation = computeOrientation();
        if (playerOrientation != m_lastMovement)
        {
            setMovementOrientation(playerOrientation, false);
        }
    }
    else
    {
        if ( m_lastMovement != IDLE)
        {
            setMovementOrientation(m_lastMovement, true);
        }
    }

}

void Player::move(const vector2D& dx)
{
    m_plannedMovement += dx;
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
    
    setAnimation(movementToAsset.at(movementOrientation + idle ? 1 : 0));
    if (idle) m_lastMovement = IDLE;
}

void Player::updateCameraPos()
{
    auto playerPos = Player::getInstance()->getPos();
    auto cameraPos = Camera::getInstance()->getPos();

    if (const auto distance = (playerPos - cameraPos).length(); distance >= allowedDistCameraToPlayer )
    {
        // compute the vector to move and update camera position
        vector2D moveVec = (playerPos - cameraPos).normalized() * (distance - allowedDistCameraToPlayer);
        Camera::getInstance()->movePos(moveVec);
    }
}