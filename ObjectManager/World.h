#pragma once

#include <list>
#include <functional>

#include "Object.h"
#include "Camera.h"

#include "SDL3/SDL.h"

using worldClickCallback = std::function<void(point2D position)>;

class World
{
public:
	World() {};

	void createCamera(point2D pos, size2D windowSize);
	void setCameraZLimits(float zMin, float zMax);
	void moveCameraPos(float dx, float dy);
	float getCameraRenderingScale();

	bool addObject(ObjectPtr object);
	bool removeObject(ObjectPtr object);
	bool addOverlayObject(ObjectPtr object);
	bool removeOverlayObject(ObjectPtr object);
	static ObjectPtr getObjectByUID(std::string uid);

	void render(SDL_Renderer* renderer);
	void update();

	void mouseMove(point2D pos);
	void Hoovering(point2D pos);

	void setLeftClickCallback(worldClickCallback& callback) { m_leftClickCallback = callback; }
	void setRightClickCallback(worldClickCallback& callback) { m_rightClickCallback = callback; }

	bool isLeftClicked() { return m_isLeftClicked; }
	virtual void onLeftClick(point2D pos);
	virtual void offLeftClick(point2D pos);
	void leftClick(point2D pos);
	void releaseLeftClick(point2D pos);


	void parseFile(std::string filePath);
	void createRenderers(SDL_Renderer* renderer);
	void saveFile(std::string filePath);

private:
	bool addobject(ObjectPtr object, std::list<ObjectPtr>& object_list);
	bool removeobject(ObjectPtr object, std::list<ObjectPtr>& object_list);

	void checkHover(ObjectPtr obj, rect2D mouse);
	bool checkLeftClick(ObjectPtr, rect2D mouse);
	bool checkLeftClickReleased(ObjectPtr, rect2D mouse);

	Camera* m_camera{ nullptr };
	std::list<ObjectPtr> m_objects{ std::list<ObjectPtr>() };
	std::list<ObjectPtr> m_overlayObjects{ std::list<ObjectPtr>() };
	static std::map<std::string, ObjectPtr> m_UIDToObjects;
	std::list<Object*> m_hooveredObjects{ std::list<Object*>() };

	bool m_isLeftClicked{ false };
	point2D m_lastClickedPos{ 0,0 };

	worldClickCallback m_leftClickCallback;
	worldClickCallback m_rightClickCallback;
};

