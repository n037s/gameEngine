#include "World.h"
#include <iostream>
#include <fstream>

#include "ObjectMemberHolder.h"
#include "ObjectManager.h"

const size2D clickingPrecisionArea = size2D(5, 5);

std::map<std::string, ObjectPtr> World::m_UIDToObjects = std::map<std::string, ObjectPtr>();

void World::createCamera(point2D pos, size2D windowSize)
{
	m_camera = Camera::getInstance();
	m_camera->setPos(pos);
	m_camera->setWindowSize(windowSize);
}

void World::setCameraZLimits(float zMin, float zMax)
{
	if (m_camera)
	{
		m_camera->setCameraZLimits(zMin, zMax);
	}
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

bool World::addobject(ObjectPtr object, std::list<ObjectPtr>& object_list)
{
	bool success = true;
	object_list.push_back(object);
	m_UIDToObjects[object->getUID()] = object;
	return success;
}

bool World::removeobject(ObjectPtr object, std::list<ObjectPtr>& object_list)
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
	
	m_UIDToObjects.erase(object->getUID());
	return success;
}

bool World::addObject(ObjectPtr object)
{
	return addobject(object, m_objects);
}
bool World::removeObject(ObjectPtr object)
{
	return removeobject(object, m_objects);
}
bool World::addOverlayObject(ObjectPtr object)
{
	return addobject(object, m_overlayObjects);
}
bool World::removeOverlayObject(ObjectPtr object)
{
	return removeobject(object, m_overlayObjects);
}

ObjectPtr World::getObjectByUID(std::string uid)
{
	ObjectPtr result = nullptr;
	if (m_UIDToObjects.find(uid) != m_UIDToObjects.end())
	{
		result = m_UIDToObjects[uid];
	}
	return result;
}

void World::render(SDL_Renderer* renderer)
{
	// retrieve objects to displays and with camera render them at good position

	rect2D renderingRect = m_camera->getRenderingRect();
	std::list<ObjectPtr> toRender = std::list<ObjectPtr>();

	// let's check all of items that are inside the rect. 
	for (ObjectPtr obj : m_objects)
	{
		if (!obj->isHidden() && renderingRect.isCollide(obj->getShape()))
		{
			rect2D renderingShape = m_camera->WorldToWindow(obj->getShape());

			obj->setRenderingRect(renderingShape);
			toRender.push_back(obj);
		}
	}
	// Sort to render list by Z value in order to display higher object on top of lower ones.
	toRender.sort([](const ObjectPtr& a, const ObjectPtr& b) {
		return *a < b.get();
		});

	// Overlay objects are on top of all objects.
	for (ObjectPtr obj : m_overlayObjects)
	{
		if (!obj->isHidden())
		{
			rect2D renderingShape = obj->getShape();

			obj->setRenderingRect(renderingShape);
			toRender.push_back(obj);
		}
	}

	// Call rendering for all objects.
	for (ObjectPtr obj : toRender)
	{
		bool success = obj->render();
	}
}
void World::update()
{
	for (ObjectPtr obj : m_objects)
	{
		obj->update();
	}
	for (ObjectPtr obj : m_overlayObjects)
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

	for (ObjectPtr obj : m_objects)
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
	for (ObjectPtr obj : m_objects)
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

	bool isAnObjectClickedOn = false;
	// Releasing click for all items
	for (ObjectPtr obj : m_objects)
	{
		bool isOnItem = mouseRectWorld.isCollide(obj->getShape());

		if (isOnItem && obj->isLeftClicked())
		{
			isAnObjectClickedOn |= obj->offLeftClick(mouseRectWorld.position);
		}
		obj->setLeftClicked(false);
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


void World::parseFile(std::string filePath)
{
	std::ifstream inFile(filePath, std::ios::binary);
	if (!inFile)
	{
		std::cout << "[Error] World::parseFile() - Cannot open file " << filePath << std::endl;
	}
	else
	{
		size_t object_size = Value::readNextValue(inFile).getValue<size_t>();

		std::cout << "---- detected " << object_size << " objects ----" << std::endl;
		for (size_t i = 0; i < object_size; ++i)
		{
			std::string objectType = Value::readNextValue(inFile).getValue<std::string>();
			std::cout << "[" << objectType << "]" << std::endl;

			// deseriailze members
			ObjectMemberHolder members;
			members.deserialize(inFile);

			addObject(ObjectManager::createObject(objectType, members));
		}
		
		size_t overlay_size = Value::readNextValue(inFile).getValue<size_t>();

		std::cout << "---- detected " << overlay_size << " overlay objects ----" << std::endl;
		for (size_t i = 0; i < overlay_size; ++i)
		{
			std::string objectType = Value::readNextValue(inFile).getValue<std::string>();
			std::cout << "[" << objectType << "]" << std::endl;

			// deseriailze members
			ObjectMemberHolder members;
			members.deserialize(inFile);

			addOverlayObject(ObjectManager::createObject(objectType, members));
		}
	}
}

void World::createRenderers(SDL_Renderer* renderer)
{
	for (ObjectPtr item : m_objects)
	{
		item->createRenderer(renderer);
	}

	for (ObjectPtr item : m_overlayObjects)
	{
		item->createRenderer(renderer);
	}
}

void World::saveFile(std::string filePath)
{
	std::ofstream outFile(filePath, std::ios::binary);
	if (!outFile)
	{
		std::cout << "[Error] World::saveFile() - Cannot open file " << filePath << std::endl;
	}
	else
	{
		// Write the world  
		Value object_size = Value(m_objects.size());
		object_size.serialize(outFile);
		for (ObjectPtr item : m_objects)
		{
			Value item_type = Value(item->getTypeName());
			item_type.serialize(outFile);
			ObjectMemberHolder members = item->serialize();
			members.serialize(outFile);
		}

		// Then write all overlays objects
		Value overlay_size = Value(m_overlayObjects.size());
		overlay_size.serialize(outFile);
		for (ObjectPtr item : m_overlayObjects)
		{
			Value item_type = Value(item->getTypeName());
			item_type.serialize(outFile);
			ObjectMemberHolder members = item->serialize();
			members.serialize(outFile);
		}

		// Maybe write the camera
	}
}