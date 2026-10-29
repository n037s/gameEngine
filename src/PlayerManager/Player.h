#pragma once

#include "AnimatedSprite.h"
#include <memory>

// It will define a player. Its will load an animation on its position. 

class Player
{
    public:
        static Player* getInstance()
        {
            if (m_instance == nullptr)
                m_instance = new Player();
            return m_instance;
        }

        void movePos(const point2D& dx);
        void setPosition(const point2D& position);
        point2D getPos() const { return m_position; }
        void setSprite(const std::shared_ptr<AnimatedSprite>& sprite);
        void setAnimation(const std::string& animation);

    private: 
        Player()
            : m_position(point2D(0, 0))
        {
        }

        static Player* m_instance;

        point2D m_position;
        std::weak_ptr<AnimatedSprite> m_sprite;
};