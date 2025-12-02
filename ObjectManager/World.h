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

	void createCamera(const point2D pos, const size2D windowSize);
	void setCameraZLimits(const float zMin, const float zMax);
	void moveCameraPos(const float dx, const float dy);
	float getCameraRenderingScale() const;

	bool addObject(const ObjectPtr object);
	bool removeObject(const ObjectPtr object);
	bool addOverlayObject(const ObjectPtr object);
	bool removeOverlayObject(const ObjectPtr object);
	static ObjectPtr getObjectByUID(const std::string& uid);

	void render(SDL_Renderer* renderer);
	void update();

	void mouseMove(const point2D pos);
	void Hoovering(const point2D pos);

	void setLeftClickCallback(worldClickCallback& callback) { m_leftClickCallback = callback; }
	void setRightClickCallback(worldClickCallback& callback) { m_rightClickCallback = callback; }

	bool isLeftClicked() const { return m_isLeftClicked; }
	virtual void onLeftClick(const point2D pos);
	virtual void offLeftClick(const point2D pos);
	void leftClick(const point2D pos);
	void releaseLeftClick(const point2D pos);

	void parseFile(const std::string& filePath);;
	void createRenderers(SDL_Renderer* renderer);
	void saveFile(const std::string& filePath);

private:
	bool addobject(const ObjectPtr object, std::list<ObjectPtr>& object_list);
	bool removeobject(const ObjectPtr object, std::list<ObjectPtr>& object_list);

	void checkHover(const ObjectPtr obj, const rect2D mouse);
	bool checkLeftClick(const ObjectPtr obj, const rect2D mouse);
	bool checkLeftClickReleased(const ObjectPtr object, const rect2D mouse);

	Camera* m_camera{ nullptr };
	std::list<ObjectPtr> m_objects{ std::list<ObjectPtr>() };
	std::list<ObjectPtr> m_overlayObjects{ std::list<ObjectPtr>() };
	static std::map<std::string, ObjectPtr> m_UIDToObjects;

	bool m_isLeftClicked{ false };
	point2D m_lastClickedPos{ 0,0 };

	worldClickCallback m_leftClickCallback;
	worldClickCallback m_rightClickCallback;
};

