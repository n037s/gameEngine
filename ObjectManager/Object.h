#pragma once

#include "Geometry.h"
#include "BaseRenderer.h"
#include "SDL3/SDL.h"


class Object
{
public:
	Object() : m_pos{ 0, 0 } {};
	Object(point2D pos) : m_pos(pos) {};
	Object(point2D pos, size2D size) : m_pos(pos), m_size(size) {};

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

	bool isHovered() { return m_isHoovered; }
	/*
	 * @brief the item begins to be hoovered
	 */
	void onHoover();
	/*
	 * @brief the button is no more hoovered
	 */
	void offHoover();

	/*
	 * @brief Hoovering item callback
	 */
	virtual void hoover();
	/*
	 * @brief No more hoovering item callback
	 */
	virtual void leftFocus();


	bool isLeftClicked() { return m_isLeftClicked; }
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
	void offLeftClick(point2D pos);
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
	virtual void releaseLeftClick (point2D pos);

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

protected:
	point2D m_pos{ 0, 0 };
	size2D m_size{ 0,0 };
	rect2D m_renderingRect{ 0,0,0,0 };
	BaseRenderer* m_renderer{ nullptr };

	bool m_isHoovered{ false };
	bool m_isLeftClicked{ false };
};

