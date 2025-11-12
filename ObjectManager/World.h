#pragma once

#include <vector>

#include "Object.h"
#include "Camera.h"

#include "SDL3/SDL.h"

class World
{
public:
	World() {};

	void createCamera(point2D pos, size2D windowSize);
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

	void wheelScrolling(float dy);

private:
	bool addobject(Object* object, std::vector<Object*>& object_list);
	bool removeobject(Object* object, std::vector<Object*>& object_list);

	Camera* m_camera{ nullptr };
	std::vector<Object*> m_objects{ std::vector<Object*>() };
	std::vector<Object*> m_overlayObjects{ std::vector<Object*>() };
	std::vector<Object*> m_hooveredObjects{ std::vector<Object*>() };

	bool m_isLeftClicked{ false };
	point2D m_lastClickedPos{ 0,0 };
};

