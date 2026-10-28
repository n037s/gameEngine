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
		const point2D dxy = m_camera->windowToWorldDelta(point2D(dx, dy));
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
	if (m_hoveredObject == object)
		m_hoveredObject = nullptr;
	return removeObject(object, m_overlayObjects);
}

bool World::isObjectInWorld()
{
	return !m_objects.empty();
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

ObjectPtr World::getObjectAtPosition(const point2D& position) const
{
	ObjectPtr result = nullptr;
	for (const ObjectPtr& object : m_objects)
	{
		if (!object->isHidden() && object->getShape().isInside(position))
			result = object;
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
	const rect2D renderingRect(point2D(0, 0), m_camera->getWindowSize());

	// let's check all of items that are inside the rect. 
	for (const ObjectPtr& obj : m_objects)
	{
		if (!obj->isHidden() && m_camera->intersectsWindowRect(obj->getShape(), renderingRect, obj->isSteady()))
		{
			if (m_camera->getViewMode() == Camera::ViewMode::Isometric)
				obj->setRenderingQuad(m_camera->getRenderingCorners(obj->getShape(), obj->isSteady()));
			else
				obj->setRenderingRect(m_camera->WorldToWindow(obj->getShape()));
			obj->render();
		}
	}
	
	// Overlay objects are on top of all objects.
	for (const ObjectPtr& obj : m_overlayObjects)
	{
		if (!obj->isHidden())
		{
			const rect2D renderingShape = obj->getShape();
			obj->setRenderingRect(renderingShape);
			obj->render();
		}
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

bool World::checkHover(const ObjectPtr obj, const rect2D mouse, bool projected)
{
	bool result = false;
	const bool isHovered = !obj->isHidden() && (projected
		? m_camera->intersectsWindowRect(obj->getShape(), mouse, obj->isSteady()) : mouse.isCollide(obj->getShape()));
	const point2D position = projected ? m_camera->windowToWorldPoint(mouse.position) : mouse.position;
	const bool wasHovered = obj->isHovered();

	if (isHovered)
	{
		obj->hover(position);
		m_hoveredObject = obj;
		result = true;
	}
	else if (wasHovered || obj->isLeftClicked())
	{
		obj->leftFocus(position);
	}
	return result;
}

void World::Hovering(const point2D pos)
{
	if (!m_camera)
		return;
	const rect2D mouseRect = rect2D(pos, clickingPrecisionArea);
	m_hoveredObject = nullptr;

	for (const ObjectPtr& obj : m_objects)
	{
		checkHover(obj, mouseRect, true);
	}
	const auto overlays = m_overlayObjects;
	for (const ObjectPtr& obj : overlays)
	{
		checkHover(obj, mouseRect);
	}
}

bool World::checkLeftClick(const ObjectPtr obj, const rect2D mouse, bool projected)
{
	bool result = false;
	const bool wasLeftClick = obj->isLeftClicked();
	const bool isLeftClick = !obj->isHidden() && (projected
		? m_camera->intersectsWindowRect(obj->getShape(), mouse, obj->isSteady()) : mouse.isCollide(obj->getShape()));
	const point2D position = projected ? m_camera->windowToWorldPoint(mouse.position) : mouse.position;

	if (!wasLeftClick && isLeftClick)
	{
		result |= obj->onLeftClick(position);
	}
	else if (wasLeftClick && !isLeftClick)
	{
		obj->offLeftClick(position);
	}
	return result;
}

void World::leftClick(const point2D pos)
{
	if (!m_camera)
		return;
	const rect2D mouseRect = rect2D(pos, clickingPrecisionArea);

	bool isAnObjectClickedOn = false;
	const auto overlays = m_overlayObjects;
	for (const auto& overlay : overlays)
		if (!overlay->isHidden() && !overlay->getShape().isInside(pos))
			overlay->outsideClick(pos);
	for (auto overlay = overlays.rbegin(); overlay != overlays.rend(); ++overlay)
	{
		if (checkLeftClick(*overlay, mouseRect))
			return;
	}

	if (redirectClickToObject)
	{
		// Check if an object is on the click
		for (const ObjectPtr& obj : m_objects)
		{
			isAnObjectClickedOn |= checkLeftClick(obj, mouseRect, true);
		}
	}

	if (!isAnObjectClickedOn && !m_isLeftClicked )
	{
		// No object have been clicked on, so it's the world left click management
		m_isLeftClicked = true;
		m_lastClickedPos = pos;
		onLeftClick(m_camera->windowToWorldPoint(pos));
	}
}

bool World::checkLeftClickReleased(const ObjectPtr object, const rect2D mouse, bool projected)
{
	bool result = false;
	const bool isOnItem = !object->isHidden() && (projected
		? m_camera->intersectsWindowRect(object->getShape(), mouse, object->isSteady()) : mouse.isCollide(object->getShape()));
	const point2D position = projected ? m_camera->windowToWorldPoint(mouse.position) : mouse.position;

	if (isOnItem && object->isLeftClicked())
	{
		result = object->offLeftClick(position);
	}
	object->setLeftClicked(false);

	return result;
}

void World::releaseLeftClick(const point2D pos)
{
	if (!m_camera)
		return;
	const rect2D mouseRect = rect2D(pos, clickingPrecisionArea);

	bool isAnObjectClickedOn = false;
	// Releasing click for all items
	for (const ObjectPtr& obj : m_objects)
	{
		isAnObjectClickedOn |= checkLeftClickReleased(obj, mouseRect, true);
	}
	const auto overlays = m_overlayObjects;
	for (const ObjectPtr& obj : overlays)
	{
		isAnObjectClickedOn |= checkLeftClickReleased(obj, mouseRect);
	}

	if (m_isLeftClicked)
	{
		// No object have been clicked on, so it's the world left click management
		m_isLeftClicked = false;
		offLeftClick(m_camera->windowToWorldPoint(pos));
	}
}

void World::onLeftClick(const point2D pos)
{
	std::cout << "World have been click on at this position : " << pos.toString() << std::endl;
}
void World::offLeftClick(const point2D pos)
{
	std::cout << "World is no more left clicked : " << pos.toString() << std::endl;
	m_leftClickCallback(pos);
}

bool World::checkRightClick(const ObjectPtr obj, const rect2D mouse, bool projected)
{
	bool result = false;
	const bool wasRightClick = obj->isRightClicked();
	const bool isRightClick = !obj->isHidden() && (projected
		? m_camera->intersectsWindowRect(obj->getShape(), mouse, obj->isSteady()) : mouse.isCollide(obj->getShape()));
	const point2D position = projected ? m_camera->windowToWorldPoint(mouse.position) : mouse.position;

	if (!wasRightClick && isRightClick)
	{
		result |= obj->onRightClick(position);
	}
	else if (wasRightClick && !isRightClick)
	{
		obj->offRightClick(position);
	}
	return result;
}

void World::rightClick(const point2D pos)
{
	if (!m_camera)
		return;
	const rect2D mouseRect = rect2D(pos, clickingPrecisionArea);

	bool isAnObjectClickedOn = false;
	const auto overlays = m_overlayObjects;
	for (const auto& overlay : overlays)
		if (!overlay->isHidden() && !overlay->getShape().isInside(pos))
			overlay->outsideClick(pos);
	for (const ObjectPtr& obj : overlays)
	{
		isAnObjectClickedOn |= checkRightClick(obj, mouseRect);
	}
	
	if (redirectClickToObject)
	{
		// Check if an object is on the click
		for (const ObjectPtr& obj : m_objects)
		{
			isAnObjectClickedOn |= checkRightClick(obj, mouseRect, true);
		}
	}

	if (!isAnObjectClickedOn && !m_isRightClicked )
	{
		// No object have been clicked on, so it's the world right click management
		m_isRightClicked = true;
		m_lastClickedPos = pos;
		onRightClick(m_camera->windowToWorldPoint(pos));
	}
}

bool World::checkRightClickReleased(const ObjectPtr object, const rect2D mouse, bool projected)
{
	bool result = false;
	const bool isOnItem = !object->isHidden() && (projected
		? m_camera->intersectsWindowRect(object->getShape(), mouse, object->isSteady()) : mouse.isCollide(object->getShape()));
	const point2D position = projected ? m_camera->windowToWorldPoint(mouse.position) : mouse.position;

	if (isOnItem && object->isRightClicked())
	{
		result = object->offRightClick(position);
	}
	object->setRightClicked(false);

	return result;
}

void World::releaseRightClick(const point2D pos)
{
	if (!m_camera)
		return;
	const rect2D mouseRect = rect2D(pos, clickingPrecisionArea);

	bool isAnObjectClickedOn = false;
	// Releasing click for all items
	for (const ObjectPtr& obj : m_objects)
	{
		isAnObjectClickedOn |= checkRightClickReleased(obj, mouseRect, true);
	}
	const auto overlays = m_overlayObjects;
	for (const ObjectPtr& obj : overlays)
	{
		isAnObjectClickedOn |= checkRightClickReleased(obj, mouseRect);
	}

	if (m_isRightClicked)
	{
		// No object have been clicked on, so it's the world right click management
		m_isRightClicked = false;
		offRightClick(m_camera->windowToWorldPoint(pos));
	}
}

void World::onRightClick(const point2D pos)
{
	std::cout << "World have been right clicked on at this position : " << pos.toString() << std::endl;
}
void World::offRightClick(const point2D pos)
{
	std::cout << "World is no more right clicked : " << pos.toString() << std::endl;
	if (m_rightClickCallback)
		m_rightClickCallback(pos);
}

bool World::scroll(float dz) const
{
	bool result = false;
	if (m_hoveredObject)
	{
		// let's try to scroll on the item 
		std::cout << "trying to scroll on the hovered item" << m_hoveredObject->getTypeName() << std::endl;
		result = m_hoveredObject->scroll(dz);
	}

	if (!result)
	{
		// let's scroll on the world
		std::cout << "scrolling on the world" << std::endl;
		result = m_wheelCallback(dz);
	}

	return result;
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