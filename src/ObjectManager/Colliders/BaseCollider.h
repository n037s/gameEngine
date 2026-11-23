#pragma once

#include "Geometry.h"

enum class ColliderType
{
    Rectangle,
    Ellipse
};

class BaseCollider
{
public:
    BaseCollider();
    virtual ~BaseCollider();

    virtual const ColliderType getType() const = 0;

    /**
     * \brief Check if the collider collides with another collider of the same type
     * \param other The other collider to check collision with
     * \return True if the colliders are colliding, false otherwise
     */
    virtual const bool collides(const BaseCollider& other) const = 0;

    const point2D& getPosition() const { return m_pos; }
    void setPosition(const point2D& pos) { m_pos = pos; }
    
private:
    point2D m_pos{ 0, 0 };

};

/**
 * \brief Checks if two colliders are colliding
 * \param a The first collider
 * \param b The second collider
 * \return True if the colliders are colliding, false otherwise
 */
bool collides(const BaseCollider& a, const BaseCollider& b);