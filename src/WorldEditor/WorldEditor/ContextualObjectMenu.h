#pragma once

#include "DropDownMenu.h"

class World;

class ContextualObjectMenu : public DropDownMenu
{
public:
	ContextualObjectMenu(ObjectPtr target, point2D position, World& world);
	~ContextualObjectMenu();

	std::string getTypeName() const override { return "ContextualObjectMenu"; }
	void leftFocus(point2D pos) override;
	void outsideClick(point2D pos) override;
	void close();

private:
	World& m_world;
	std::string m_callbackID;
	std::vector<std::string> m_assetCallbackIDs;
};