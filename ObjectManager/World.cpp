#include "World.h"
#include <iostream>

const size2D clickingPrecisionArea = size2D(5, 5);

void World::createCamera(point2D pos, size2D windowSize)
{
	m_camera = Camera::getInstance();
	m_camera->setPos(pos);
	m_camera->setWindowSize(windowSize);
}

void World::moveCameraPos(float dx, float dy)
{
	point2D dxy = point2D(dx, dy) / m_camera->getRenderingScale();
	m_camera->setPos(m_camera->getPos() + dxy);
}

float World::getCameraRenderingScale()
{
	return m_camera->getRenderingScale();
}

bool World::addobject(Object* object, std::vector<Object*>& object_list)
{
	bool success = true;
	object_list.push_back(object);
	return success;
}

bool World::removeobject(Object* object, std::vector<Object*>& object_list)
{
	bool success = false;

	auto target = std::find(object_list.begin(), object_list.end(), object);
	if (target != object_list.end())
	{
		object_list.erase(target);
		success = true;
	}
	else
		std::cout << "Object is not found to deletion" << std::endl;

	return success;
}

bool World::addObject(Object* object)
{
	return addobject(object, m_objects);
}
bool World::removeObject(Object* object)
{
	return removeobject(object, m_objects);
}
bool World::addOverlayObject(Object* object)
{
	return addobject(object, m_overlayObjects);
}
bool World::removeOverlayObject(Object* object)
{
	return removeobject(object, m_overlayObjects);
}

void World::render(SDL_Renderer* renderer)
{
	// retrieve objects to displays and with camera render them at good position

	rect2D renderingRect = m_camera->getRenderingRect();
	// let's check all of items that are inside the rect. 
	for (Object* obj : m_objects)
	{
		if (!obj->isHidden() && renderingRect.isCollide(obj->getShape()))
		{
			rect2D renderingShape = m_camera->WorldToWindow(obj->getShape());
			std::cout << "Initial shape : " << obj->getShape().toString() << " -> " << renderingShape.toString() << std::endl;

			obj->setRenderingRect(renderingShape);
			bool success = obj->render();
		}
	}
	for (Object* obj : m_overlayObjects)
	{
		if (!obj->isHidden())
		{
			rect2D renderingShape = obj->getShape();

			obj->setRenderingRect(renderingShape);
			bool success = obj->render();
		}
	}
}
void World::update()
{
	for (Object* obj : m_objects)
	{
		obj->update();
	}
	for (Object* obj : m_overlayObjects)
	{
		obj->update();
	}
}

void World::mouseMove(point2D pos)
{
	if (m_isLeftClicked)
	{
		// World grabbing
		point2D d_pos = m_lastClickedPos - pos;
		m_lastClickedPos = pos;
		moveCameraPos(d_pos.x, d_pos.y);
	}
	else
	{
		// Else, check if we are hoovering something
		Hoovering(pos);
	}
}

void World::Hoovering(point2D pos)
{
	rect2D mouseRect = rect2D(pos, clickingPrecisionArea); // in window 
	rect2D mouseRectWorld = m_camera->WindowToWorld(mouseRect); // in world

	for (Object* obj : m_objects)
	{
		bool wasHoovered = obj->isHovered();
		bool isHoovered = mouseRectWorld.isCollide(obj->getShape());

		if (!wasHoovered && isHoovered)
		{
			obj->onHoover();
		}
		else if (wasHoovered && !isHoovered)
		{
			obj->offHoover();
		}
	}
}

void World::leftClick(point2D pos)
{
	rect2D mouseRect = rect2D(pos, clickingPrecisionArea); // in window 
	rect2D mouseRectWorld = m_camera->WindowToWorld(mouseRect); // in world

	bool isAnObjectClickedOn = false;
	// Check if an object is on the click
	for (Object* obj : m_objects)
	{
		bool wasLeftClick = obj->isLeftClicked();
		bool isLeftClick = mouseRectWorld.isCollide(obj->getShape());

		if (!wasLeftClick && isLeftClick)
		{
			isAnObjectClickedOn |= obj->onLeftClick(mouseRectWorld.position);
		}
		else if (wasLeftClick && !isLeftClick)
		{
			obj->offLeftClick(mouseRectWorld.position);
		}
	}

	if (!isAnObjectClickedOn && !m_isLeftClicked)
	{
		// No object have been clicked on, so it's the world left click management
		m_isLeftClicked = true;
		m_lastClickedPos = pos;
		onLeftClick(mouseRectWorld.position);
	}
}

void World::releaseLeftClick(point2D pos)
{
	rect2D mouseRect = rect2D(pos, clickingPrecisionArea); // in window 
	rect2D mouseRectWorld = m_camera->WindowToWorld(mouseRect); // in world

	// Releasing click for all items
	for (Object* obj : m_objects)
	{
		if (obj->isLeftClicked())
		{
			obj->offLeftClick(mouseRectWorld.position);
		}
	}

	if (m_isLeftClicked)
	{
		// No object have been clicked on, so it's the world left click management
		m_isLeftClicked = false;
		offLeftClick(mouseRectWorld.position);
	}
}

void World::onLeftClick(point2D pos)
{
	std::cout << "World have been click on at this position : " << pos.toString() << std::endl;
}
void World::offLeftClick(point2D pos)
{
	std::cout << "World is no more left clicked : " << pos.toString() << std::endl;
}

void World::wheelScrolling(float dy)
{
	const float wheelRatio = -0.2;
	m_camera->moveZ(wheelRatio*dy);
}