#pragma once
#include "Object.h"

#include "DropDownMenuRenderer.h"

struct Item
{
	Item(std::string label, bool isSubMenu, std::string callbackID = "") : label(label), isSubMenu(isSubMenu), callbackID(callbackID) 
	{
		if (isSubMenu)
		{
			isOpen = false;
			items = std::vector<std::shared_ptr<Item>>();
		}
	};

	std::string label; 
	bool isSubMenu;
	
	// if isSubMenu :
	bool isOpen;
	std::vector<std::shared_ptr<Item>> items;
	size_t firstVisibleItem{ 0 };

	// if not SubMenu :
	std::string callbackID;
};

using ItemPtr = std::shared_ptr<Item>;

class DropDownMenu : public Object
{
public:
	DropDownMenu(const ObjectMemberHolder& members);
	DropDownMenu(rect2D shape, size2D itemSize, Color buttonColor, FontPtr font);

	bool createRenderer(SDL_Renderer* renderer) override;

	std::string getTypeName() const override { return "DropDownMenu"; }
	ObjectMemberHolder serialize() const override;

	void addItem(const std::string& ID, const std::string& callbackID = "Nothing");
	ItemPtr getChildItem(const std::string& label, ItemPtr parent) const;
	void setCascading(bool cascading) { m_cascading = cascading; m_cascadeOrigin = m_pos; }
	void sortItems();

	void display(ItemPtr menu, int level = 0, bool isMenuChanged = false);

	void updateRenderer();
	void hover(point2D pos) override;
	void leftFocus(point2D pos) override;
	bool leftClick(point2D pos) override;
	bool releaseLeftClick(point2D pos) override;

	bool scroll(float dz) override;

	void printMenu(ItemPtr menu, int level = 0);

private:
	ItemPtr m_items;
	bool m_cascading{ false };
	point2D m_cascadeOrigin;
	ItemPtr m_hoveredMenu;
	std::vector<std::pair<ObjectPtr, ItemPtr>> m_visibleItems;
	void displayColumn(ItemPtr menu, point2D position, float parentWidth = 0);
	void sortItems(ItemPtr menu);
	void closeSubMenus(ItemPtr menu);

	void addItemButton(std::string label, std::string callbackID, size2D size = size2D());
	void addSubMenuButton(std::string label, ItemPtr item);

	void updateRendererMenu();

	std::list<ObjectPtr> m_objectList;
	size2D m_itemSize;
	Color m_buttonColor;
	FontPtr m_font;

	SDL_Renderer* sdl_renderer{ nullptr };
};

using DropDownMenuPtr = std::shared_ptr<DropDownMenu>;
