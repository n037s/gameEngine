#pragma once
#include <iostream>
#include <vector>

#include "Geometry.h"
#include "BaseRenderer.h"
#include "ObjectMemberHolder.h"
#include "UIDGenerator.h"

#include "SDL3/SDL.h"

class Object
{
public:
	Object(const ObjectMemberHolder& members);
	Object(point2D pos = point2D(0, 0), size2D size = size2D(1, 1), float z = 0) : m_pos(pos), m_size(size), m_z(z) {}
	~Object();

	bool isHidden() const { return m_isHidden; }
	void setIsHidden(bool isHidden) { m_isHidden = isHidden; }

	const std::string& getUID() const { return m_uid; }
	virtual std::string getTypeName() const = 0;
	virtual ObjectMemberHolder serialize() const;

	/*
	 * @brief create the item renderer
	 *
	 * @param renderer - the renderer of the window
	 * 
	 * @return bool if a callback is performed successfully
	 */
	virtual bool createRenderer(SDL_Renderer* renderer) = 0;

	/*
	 * @brief render the item
	 *
	 * @return bool if the rendering is successfull
	 */
	virtual bool render();
	/*
	 * @brief the item is updated at each frame rate
	 */
	virtual void update();

	bool isHovered() const { return m_isHovered; }
	/*
	 * @brief the item begins to be hovered
	 */
	virtual void onHover(point2D pos);
	/*
	 * @brief the button is no more hovered
	 */
	virtual void offHover(point2D pos);

	/*
	 * @brief the item is hovered
	 */
	virtual void hover(point2D pos);
	/*
	 * @brief the item is no more hovered
	 */
	virtual void leftFocus(point2D pos);

	void setLeftClicked(bool isLeftClicked) { m_isLeftClicked = isLeftClicked; }
	bool isLeftClicked() const { return m_isLeftClicked; }
	/*
	 * @brief callback of the item when it is left clicked on
	 * 
	 * @param pos - position of the click
	 * 
	 * @return bool if a callback have been performed
	 */
	bool onLeftClick(point2D pos);
	/*
	 * @brief the button is no more left clicked on
	 * 
	 * @param pos - position of the click
	 * 
	 * @return bool if a callback have been performed
	 */
	bool offLeftClick(point2D pos);
	/*
	 * @brief leftClick item callback
	 * 
	 * @param pos - position of the click
	 * 
	 * @return bool if a callback have been performed
	 */
	virtual bool leftClick(point2D pos);
	/*
	 * @brief leftClick have been released
	 * 	
	 * @param pos - position of the click
	 * 
	 * @return bool if a callback have been performed
	 */
	virtual bool releaseLeftClick(point2D pos);

	/*
	 * @brief scroll on the item
	 *
	 * @param dz - scrolling dz
	 *
	 * @return bool if a callback have been performed
	 */
	virtual bool scroll(float dz);

	/*
	 * @brief get the position of the item
	 *
	 * @return point2D position of the item
	 */
	virtual const point2D getPos() const { return m_pos; }

	/*
	 * @brief get the shape and the position of the item
	 *
	 * @return rect2D position and shape of the item
	 */
	virtual const rect2D getShape() const { return rect2D(m_pos, m_size); };

	void setRenderingRect(rect2D renderingRect) { m_renderingRect = renderingRect; }

	void setZ(float z) { m_z = z; }
	float getZ() const { return m_z; }

	BaseRenderer* getRenderer() const { return m_renderer; }

	bool operator>(const Object* other) const { return (m_z > other->m_z); }
	bool operator<(const Object* other) const { return (m_z < other->m_z); }
	bool operator<=(const Object* other) const { return (m_z <= other->m_z); }
	bool operator>=(const Object* other) const { return (m_z >= other->m_z); }
	bool operator==(const Object* other) const { return (m_z == other->m_z); }

protected:
	std::string m_uid{ UIDGenerator::generateUID() };

	ObjectMemberHolder m_members;
	point2D m_pos{ 0, 0 };
	float m_z{ 0 };
	size2D m_size{ 0,0 };
	rect2D m_renderingRect{ 0,0,0,0 };
	BaseRenderer* m_renderer{ nullptr };

	bool m_isHidden{ false };

	bool m_isHovered{ false };
	bool m_isLeftClicked{ false };
};


using ObjectPtr = std::shared_ptr<Object>;