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

private:
	Camera* m_camera{ nullptr };
	std::vector<Object*> m_objects{ std::vector<Object*>() };
};

