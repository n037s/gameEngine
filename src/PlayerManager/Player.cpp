#include "Player.h"
#include "Camera.h"

Player* Player::m_instance = nullptr;

void Player::movePos(const point2D& dx)
{
    m_position.x += dx.x;
    m_position.y += dx.y;

    if (const auto sprite = m_sprite.lock())
        sprite->setPos(m_position);
}

void Player::setPosition(const point2D& position)
{
    m_position = position;
    if (const auto sprite = m_sprite.lock())
        sprite->setPos(m_position);
}

void Player::setSprite(const std::shared_ptr<AnimatedSprite>& sprite)
{
    m_sprite = sprite;
    if (sprite)
        m_position = sprite->getPos();
}

void Player::setAnimation(const std::string& animation)
{
    std::cout << "Setting animation to: " << animation << std::endl;
    if (const auto sprite = m_sprite.lock())
        sprite->setAnimation(animation);
    else
        std::cout << "No sprite available to set animation." << std::endl;
}