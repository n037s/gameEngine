#pragma once

#include "Object.h"
#include "BaseController.h"
#include "SelectionBoundRenderer.h"

class ContextualObjectMenu;

enum ObjectControlTimeout
{
    MOVE_LEFT = 0,
    MOVE_DOWN,
    MOVE_RIGHT,
    MOVE_UP
};

enum EdgeType
{
    NONE = 0, 
    TOP_LEFT,
    TOP,
    TOP_RIGHT,
    RIGHT,
    BOTTOM_RIGHT,
    BOTTOM,
    BOTTOM_LEFT,
    LEFT
};

enum EdgeAction
{
	None = 0,
	Grab, 
	Resize_NE,
	Resize_NW,
	Resize_SE,
	Resize_SW,
	Resize_N,
	Resize_S,
	Resize_E,
	Resize_W
};


class SelectionBoundsObject final : public Object, public BaseController
	{
	public:
		explicit SelectionBoundsObject(ObjectPtr target) { setTarget(std::move(target)); }

		std::string getTypeName() const override;

		bool createRenderer(SDL_Renderer* renderer) override;
		bool render() override;

		const rect2D getShape() const override;

		void setTarget(ObjectPtr object);

		bool leftClick(point2D pos) override;
		bool releaseLeftClick(point2D pos) override;
		void hover(point2D pos) override;
		void leftFocus(point2D pos) override;

		bool rightClick(point2D pos) override;
		bool releaseRightClick(point2D pos) override;

	private:
		void setupCallbacks();
		point2D toTargetSpace(point2D windowPos) const;
		void dragTarget(point2D windowPos);
		void updateCursor(point2D pos);

		const EdgeType getEdge(point2D pos) const;

		ObjectPtr target;
		std::weak_ptr<ContextualObjectMenu> m_contextMenu;
		point2D m_grabOffset{ 0, 0 };
		EdgeAction m_edgeAction{ EdgeAction::None };
	};