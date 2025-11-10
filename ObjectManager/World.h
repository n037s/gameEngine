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

	bool addObject(Object* object);

	void render(SDL_Renderer* renderer);

	void mouseMove(point2D pos);
	void Hoovering(point2D pos);

	bool isLeftClicked() { return m_isLeftClicked; }
	virtual void onLeftClick(point2D pos);
	virtual void offLeftClick(point2D pos);
	void leftClick(point2D pos);
	void releaseLeftClick(point2D pos);

private:

	Camera* m_camera{ nullptr };
	std::vector<Object*> m_objects{ std::vector<Object*>() };
	std::vector<Object*> m_hooveredObjects{ std::vector<Object*>() };

	bool m_isLeftClicked{ false };
	point2D m_lastClickedPos{ 0,0 };
};

