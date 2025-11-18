#pragma once

#include <list>

#include "Object.h"
#include "Camera.h"

#include "SDL3/SDL.h"

class World
{
public:
	World() {};

	void createCamera(point2D pos, size2D windowSize);
	void setCameraZLimits(float zMin, float zMax);
	void moveCameraPos(float dx, float dy);
	float getCameraRenderingScale();

	bool addObject(Object* object);
	bool removeObject(Object* object);
	bool addOverlayObject(Object* object);
	bool removeOverlayObject(Object* object);

	void render(SDL_Renderer* renderer);
	void update();

	void mouseMove(point2D pos);
	void Hoovering(point2D pos);

	bool isLeftClicked() { return m_isLeftClicked; }
	virtual void onLeftClick(point2D pos);
	virtual void offLeftClick(point2D pos);
	void leftClick(point2D pos);
	void releaseLeftClick(point2D pos);


	void parseFile(std::string filePath);
	void saveFile(std::string filePath);

private:
	bool addobject(std::shared_ptr<Object> object, std::list<std::shared_ptr<Object>>& object_list);
	bool removeobject(std::shared_ptr<Object> object, std::list<std::shared_ptr<Object>>& object_list);

	Camera* m_camera{ nullptr };
	std::list<std::shared_ptr<Object>> m_objects{ std::list<std::shared_ptr<Object>>() };
	std::list<std::shared_ptr<Object>> m_overlayObjects{ std::list<std::shared_ptr<Object>>() };
	std::list<Object*> m_hooveredObjects{ std::list<Object*>() };

	bool m_isLeftClicked{ false };
	point2D m_lastClickedPos{ 0,0 };
};

