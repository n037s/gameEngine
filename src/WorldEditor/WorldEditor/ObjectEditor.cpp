#include "ObjectEditor.h"

#include "Camera.h"
#include "Viewer.h"
#include "World.h"
#include "SelectionBoundObject.h"

#include <algorithm>
#include <utility>

ObjectEditor* ObjectEditor::instance = nullptr;

ObjectEditor& ObjectEditor::getInstance()
{
	if (!instance)
		instance = new ObjectEditor();
	return *instance;
}

ObjectEditor::ObjectEditor() = default;

void ObjectEditor::updateSelectedObject(point2D position)
{
    std::cout << "update selected object" << std::endl;
	ObjectEditor& editor = getInstance();
	Viewer* viewer = Viewer::getInstance();
	World* world = viewer->getWorld();
	editor.selectedObject = world ? world->getObjectAtPosition(position) : nullptr;
	editor.updateBoundaries();
}

void ObjectEditor::updateBoundaries()
{
	if (!selectedObject)
	{
		if (boundariesObject)
			boundariesObject->setIsHidden(true);
		return;
	}

	Viewer* viewer = Viewer::getInstance();
	World* world = viewer->getWorld();
	if (!world)
		return;

	if (!boundariesObject)
	{
		auto bounds = std::make_shared<SelectionBoundsObject>(selectedObject);
		bounds->setZ(100000.0f);
		if (!bounds->createRenderer(viewer->getRenderer()))
			return;
		boundariesObject = bounds;
		world->addOverlayObject(boundariesObject);
	}
	else
	{
		auto bounds = std::static_pointer_cast<SelectionBoundsObject>(boundariesObject);
		bounds->setTarget(selectedObject);
		boundariesObject->setIsHidden(false);
	}
}
