#include "World.h"
#include <iostream>


void World::createCamera(point2D pos, size2D windowSize)
{
	m_camera = Camera::getInstance();
	m_camera->setPos(pos);
	m_camera->setWindowSize(windowSize);
}

void World::moveCameraPos(float dx, float dy)
{
	point2D dxy = point2D(dx, dy);
	m_camera->setPos(m_camera->getPos() + dxy);
}

bool World::addObject(Object* object)
{
	bool success = true;
	m_objects.push_back(object);
	return success;
}

void World::render(SDL_Renderer* renderer)
{
	// retrieve objects to displays and with camera render them at good position

	rect2D renderingRect = m_camera->getRenderingRect();
	// let's check all of items that are inside the rect. 
	for (Object* obj : m_objects)
	{
		if (renderingRect.isCollide(obj->getShape()))
		{
			rect2D renderingShape = m_camera->computeRenderingRect(obj->getShape());

			obj->setRenderingRect(renderingShape);
			bool success = obj->render();
		}
	}
}