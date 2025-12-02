	#include "World.h"
#include <iostream>
#include <fstream>

#include "ObjectMemberHolder.h"
#include "ObjectManager.h"

const size2D clickingPrecisionArea = size2D(5, 5);

void World::createCamera(const point2D pos, const size2D windowSize)
{
	m_camera = Camera::getInstance();
	m_camera->setPos(pos);
	m_camera->setWindowSize(windowSize);
}

void World::setCameraZLimits(const float zMin, const float zMax)
{
	if (m_camera)
	{
		m_camera->setCameraZLimits(zMin, zMax);
	}
}

void World::moveCameraPos(const float dx, const float dy)
{
	if (m_camera)
	{
		const point2D dxy = point2D(dx, dy) / m_camera->getRenderingScale();
		m_camera->setPos(m_camera->getPos() + dxy);
	}
}

float World::getCameraRenderingScale() const
{
	float result = 1;
	if (m_camera)
		result = m_camera->getRenderingScale();
	return result;
}

bool World::addObject(const ObjectPtr object, std::list<ObjectPtr>& object_list)
{
	bool success = true;
	object_list.push_back(object);
	m_UIDToObjects[object->getUID()] = object;
	sortObjectListByZ(object_list);
	return success;
}

bool World::removeObject(const ObjectPtr object, std::list<ObjectPtr>& object_list)
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
	sortObjectListByZ(object_list);
	return success;
}

bool World::addObject(const ObjectPtr object)
{
	return addObject(object, m_objects);
}
bool World::removeObject(const ObjectPtr object)
{
	return removeObject(object, m_objects);
}
bool World::addOverlayObject(const ObjectPtr object)
{
	return addObject(object, m_overlayObjects);
}
bool World::removeOverlayObject(const ObjectPtr object)
{
	return removeObject(object, m_overlayObjects);
}

ObjectPtr World::getObjectByUID(const std::string& uid)
{
	ObjectPtr result = nullptr;
	if (m_UIDToObjects.find(uid) != m_UIDToObjects.end())
	{
		result = m_UIDToObjects[uid];
	}
	return result;
}

void World::sortObjectsByZ()
{
	sortObjectListByZ(m_objects);
	sortObjectListByZ(m_overlayObjects);
}

void World::sortObjectListByZ(std::list<ObjectPtr>& object_list)
{
	object_list.sort([](const ObjectPtr& a, const ObjectPtr& b) {
		return *a < b.get();
	});
}

void World::render(SDL_Renderer* renderer)
{
	// preconditions
	if (!m_camera)
		return;
	
	// retrieve objects to displays and with camera render them at good position
	const rect2D renderingRect = m_camera->getRenderingRect();
	std::list<ObjectPtr> toRender;

	// let's check all of items that are inside the rect. 
	for (const ObjectPtr& obj : m_objects)
	{
		if (!obj->isHidden() && renderingRect.isCollide(obj->getShape()))
		{
			const rect2D renderingShape = m_camera->WorldToWindow(obj->getShape());
			obj->setRenderingRect(renderingShape);
			toRender.push_back(obj);
		}
	}
	
	// Overlay objects are on top of all objects.
	for (const ObjectPtr& obj : m_overlayObjects)
	{
		if (!obj->isHidden())
		{
			const rect2D renderingShape = obj->getShape();
			obj->setRenderingRect(renderingShape);
			toRender.push_back(obj);
		}
	}

	// Call rendering for all objects.
	for (const ObjectPtr& obj : toRender)
	{
		bool success = obj->render();
	}
}
void World::update()
{
	for (const ObjectPtr& obj : m_objects)
	{
		obj->update();
	}
	for (const ObjectPtr& obj : m_overlayObjects)
	{
		obj->update();
	}
}

void World::mouseMove(const point2D pos)
{
	if (m_isLeftClicked)
	{
		// World grabbing
		const point2D d_pos = m_lastClickedPos - pos;
		m_lastClickedPos = pos;
		moveCameraPos(d_pos.x, d_pos.y);
	}
	else
	{
		// Else, check if we are hovering something
		Hovering(pos);
	}
}

void World::checkHover(const ObjectPtr obj, const rect2D mouse)
{
	const bool isHovered = mouse.isCollide(obj->getShape());

	if (isHovered)
	{
		obj->hover(mouse.position);
	}
	else if (!isHovered)
	{
		obj->leftFocus(mouse.position);
	}
}

void World::Hovering(const point2D pos)
{
	const rect2D mouseRect = rect2D(pos, clickingPrecisionArea);
	const rect2D mouseRectWorld = m_camera->WindowToWorld(mouseRect);

	for (const ObjectPtr& obj : m_objects)
	{
		checkHover(obj, mouseRectWorld);
	}
	for (const ObjectPtr& obj : m_overlayObjects)
	{
		checkHover(obj, mouseRect);
	}
}

bool World::checkLeftClick(const ObjectPtr obj, const rect2D mouse)
{
	bool result = false;
	const bool wasLeftClick = obj->isLeftClicked();
	const bool isLeftClick = mouse.isCollide(obj->getShape());

	if (!wasLeftClick && isLeftClick)
	{
		result |= obj->onLeftClick(mouse.position);
	}
	else if (wasLeftClick && !isLeftClick)
	{
		obj->offLeftClick(mouse.position);
	}
	return result;
}

void World::leftClick(const point2D pos)
{
	const rect2D mouseRect = rect2D(pos, clickingPrecisionArea);
	const rect2D mouseRectWorld = m_camera->WindowToWorld(mouseRect);

	bool isAnObjectClickedOn = false;
	// Check if an object is on the click
	for (const ObjectPtr& obj : m_objects)
	{
		isAnObjectClickedOn |= checkLeftClick(obj, mouseRectWorld);
	}
	for (const ObjectPtr& obj : m_overlayObjects)
	{
		isAnObjectClickedOn |= checkLeftClick(obj, mouseRect);
	}

	if (!isAnObjectClickedOn && !m_isLeftClicked)
	{
		// No object have been clicked on, so it's the world left click management
		m_isLeftClicked = true;
		m_lastClickedPos = pos;
		onLeftClick(mouseRectWorld.position);
	}
}

bool World::checkLeftClickReleased(const ObjectPtr object, const rect2D mouse)
{
	bool result = false;
	const bool isOnItem = mouse.isCollide(object->getShape());

	if (isOnItem && object->isLeftClicked())
	{
		result = object->offLeftClick(mouse.position);
	}
	object->setLeftClicked(false);

	return result;
}

void World::releaseLeftClick(const point2D pos)
{
	const rect2D mouseRect = rect2D(pos, clickingPrecisionArea);
	const rect2D mouseRectWorld = m_camera->WindowToWorld(mouseRect);

	bool isAnObjectClickedOn = false;
	// Releasing click for all items
	for (const ObjectPtr& obj : m_objects)
	{
		isAnObjectClickedOn |= checkLeftClickReleased(obj, mouseRectWorld);
	}
	for (const ObjectPtr& obj : m_overlayObjects)
	{
		isAnObjectClickedOn |= checkLeftClickReleased(obj, mouseRect);
	}

	if (m_isLeftClicked)
	{
		// No object have been clicked on, so it's the world left click management
		m_isLeftClicked = false;
		offLeftClick(mouseRectWorld.position);
	}
}

void World::onLeftClick(const point2D pos)
{
	std::cout << "World have been click on at this position : " << pos.toString() << std::endl;
}
void World::offLeftClick(const point2D pos)
{
	std::cout << "World is no more left clicked : " << pos.toString() << std::endl;
}

void World::parseFile(const std::string& filePath)
{
	std::ifstream inFile(filePath, std::ios::binary);
	if (!inFile)
	{
		std::cout << "[Error] World::parseFile() - Cannot open file " << filePath << std::endl;
	}
	else
	{
		const size_t object_size = Value::readNextValue(inFile).getValue<size_t>();

		std::cout << "---- detected " << object_size << " objects ----" << std::endl;
		for (size_t i = 0; i < object_size; ++i)
		{
			const std::string objectType = Value::readNextValue(inFile).getValue<std::string>();
			std::cout << "[" << objectType << "]" << std::endl;

			ObjectMemberHolder members;
			members.deserialize(inFile);
			if (auto object = ObjectManager::createObject(objectType, members))
				addObject(object);
		}
	}
}

void World::createRenderers(SDL_Renderer* renderer)
{
	for (const ObjectPtr& item : m_objects)
	{
		item->createRenderer(renderer);
	}

	for (const ObjectPtr& item : m_overlayObjects)
	{
		item->createRenderer(renderer);
	}
}

void World::saveFile(const std::string& filePath)
{
	std::ofstream outFile(filePath, std::ios::binary);
	if (!outFile)
	{
		std::cout << "[Error] World::saveFile() - Cannot open file " << filePath << std::endl;
	}
	else
	{
		Value object_size = Value(m_objects.size());
		object_size.serialize(outFile);
		for (const ObjectPtr& item : m_objects)
		{
			Value item_type = Value(item->getTypeName());
			item_type.serialize(outFile);
			ObjectMemberHolder members = item->serialize();
			members.serialize(outFile);
		}
	}
}