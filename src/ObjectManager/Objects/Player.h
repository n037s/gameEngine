#pragma once

#include "AnimatedSprite.h"
#include <memory>

namespace PlayerMovement
{
    enum Direction
    {
        IDLE = 0, // General IDLE state
        MOVE_RIGHT,
        IDLE_RIGHT,
        MOVE_LEFT,
        IDLE_LEFT,
        MOVE_UP,
        IDLE_UP,
        MOVE_DOWN,
        IDLE_DOWN
    };
}

class Player : public AnimatedSprite
{
    private: 
        struct PlayerSingletonToken {};

    public:
        static std::shared_ptr<Player> getInstance(const ObjectMemberHolder& members = ObjectMemberHolder())
        {
            static std::shared_ptr<Player> instance = members.isEmpty() ? 
                std::make_shared<Player>(PlayerSingletonToken{}) : 
                std::make_shared<Player>(PlayerSingletonToken{}, members);
            return instance;
        }
        Player(PlayerSingletonToken pst);
        Player(PlayerSingletonToken pst, const ObjectMemberHolder& members );

        std::string getTypeName() const override { return "Player"; }
        void update() override;

        /**
         * \brief Planify a move
         * \param dx vector of the movement to plan
         */
        void move(const vector2D& dx);
        void setPos(const point2D& position) override;

    private: 
        const PlayerMovement::Direction computeOrientation();
        void setMovementOrientation(const PlayerMovement::Direction& movementOrientation, bool idle);
        void updateCameraPos();

        bool m_inMovement{false};
        vector2D m_plannedMovement{0,0};
        PlayerMovement::Direction m_lastMovement{PlayerMovement::IDLE};
};