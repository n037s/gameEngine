#include "DropDownMenu.h"

#include "ButtonCallbackRegistery.h"
#include "Button.h"

#include <algorithm>
#include <cctype>
#include <sstream>


DropDownMenu::DropDownMenu(const ObjectMemberHolder& members) : Object(members)
{
	// Retrieve the number of items
	// For the number of items : 
	// retrieve the type of the item to create
	// create the item through the factory and add it to the list.
}

DropDownMenu::DropDownMenu(rect2D shape, size2D itemSize, Color buttonColor, FontPtr font) : Object(shape.position, shape.size)
{
	m_itemSize = itemSize;
	m_buttonColor = buttonColor;
	m_font = font;
}

ObjectMemberHolder DropDownMenu::serialize() const
{
	ObjectMemberHolder objectMemberHolder = Object::serialize();
	objectMemberHolder.addMember("DropDownItemCount", m_objectList.size());
	int i = 1;
	for (auto item : m_objectList)
	{
		objectMemberHolder.addMember("DropDownItemType" + i, item->getTypeName());
		
		ObjectMemberHolder itemMembers = item->serialize();
		// Need to go throught all items of itemMembers. Need access to the map.
	}
	objectMemberHolder.addMember("DropDownItemSize", m_itemSize);
	objectMemberHolder.addMember("DropDownItemButtonColor", m_buttonColor);
	return objectMemberHolder;
}

bool DropDownMenu::createRenderer(SDL_Renderer* renderer)
{
	bool res = true;
	sdl_renderer = renderer;
	DropDownMenuRenderer* dropDownMenu = new DropDownMenuRenderer(sdl_renderer, m_size, m_itemSize, m_buttonColor.toSDL());
	if (dropDownMenu)
	{
		m_renderer = dropDownMenu;
		display(nullptr, 0, true);
	}
	else
		res = false;

	return res;
}


void DropDownMenu::addItemButton(std::string label, std::string callbackID, size2D size)
{
	int itemCount = static_cast<int>(m_objectList.size());
	size2D dy = m_itemSize * size2D(0, itemCount);
	point2D position = m_pos + dy;

	ObjectMemberHolder members;
	members.addMember("UID", UIDGenerator::generateUID());
	members.addMember("objectPosition", position);
	members.addMember("objectHeight", m_z);
	members.addMember("objectSize", size.w > 0 ? size : m_itemSize);
	members.addMember("isObjectHidden", m_isHidden);
	members.addMember("buttonLabel", label);
	members.addMember("buttonColor", m_buttonColor);
	members.addMember("buttonLabelFont", m_font);
	members.addMember("buttonLabelFontColor", Color(0,0,0,255));
	members.addMember("butonCallbackOnClick", std::string("Nothing"));
	members.addMember("buttonCallbackOnRelease", callbackID);
	
	ObjectPtr object = std::make_shared<Button>(members);
	m_objectList.push_back(object);
}

void DropDownMenu::addSubMenuButton(std::string label, ItemPtr item)
{
	int itemCount = m_objectList.size();
	size2D dy = m_itemSize * size2D(0, itemCount);
	point2D position = m_pos + dy;

	// Let's create s button that displays next to it "object" item when clicked on.
	ObjectMemberHolder members;
	members.addMember("UID", UIDGenerator::generateUID());
	members.addMember("objectPosition", position);
	members.addMember("objectHeight", m_z);
	members.addMember("objectSize", m_itemSize);
	members.addMember("isObjectHidden", m_isHidden);
	members.addMember("buttonLabel", label);
	members.addMember("buttonColor", m_buttonColor);
	members.addMember("buttonLabelFont", m_font);
	members.addMember("buttonLabelFontColor", Color(0, 0, 0, 255));
	members.addMember("butonCallbackOnClick", std::string("Nothing"));
	// We can't save the function in the file ... 
	std::string func_name = "DropDownCallback" + UIDGenerator::generateUID();
	ButtonCallbackRegistery::instance()->registerCallback(func_name, [this, item](point2D pos, Object* caller) {
		std::cout << "SUBMENU is clicked on, menu list before : " << std::endl;
		printMenu(nullptr);
		item->isOpen = !item->isOpen;
		this->display(nullptr, 0, true);
		std::cout << "SUBMENU is clicked on, menu list after : " << std::endl;
		printMenu(nullptr);
		return true;
		});
	members.addMember("buttonCallbackOnRelease", func_name);

	ObjectPtr object = std::make_shared<Button>(members);
	m_objectList.push_back(object);
}

void DropDownMenu::updateRendererMenu()
{
	auto renderer = dynamic_cast<DropDownMenuRenderer*>(m_renderer);
	if (renderer)
	{
		renderer->clearItems();
		renderer->setSize(m_size);
		for (auto item : m_objectList)
		{
			item->createRenderer(sdl_renderer);
			auto item_renderer = dynamic_cast<ButtonRenderer*>(item->getRenderer());
			if (item_renderer)
				renderer->addItem(item_renderer, item->getPos() - m_pos);
		}
		renderer->update();
	}
}

void DropDownMenu::addItem(const std::string& ID, const std::string& callbackID)
{
	// ID is as subMenu1/subMenu2/item
	// First if the structure is not created, let's create it.
	if (!m_items)
	{
		m_items = std::make_shared<Item>("main", true);
	}
	
	// First let's decompose all subMenu to get the final subMenu :
	std::stringstream ss(ID);
	std::string word;

	const size_t ID_words_count = std::count(ID.begin(), ID.end(), '/');
	int current_word_iterator = 0;
	ItemPtr node = m_items;

	while (std::getline(ss, word, '/')) {
		if (current_word_iterator++ < ID_words_count)
		{
			// let's see if the subMenu exists
			ItemPtr child_node = getChildItem(word, node);
			if (!child_node)
			{
				ItemPtr new_node = std::make_shared<Item>(word, true);
				node->items.push_back(new_node);
				node = new_node;
			}
			else
			{
				// subMenu already exists. Let's continue...
				node = child_node;
			}
		}
		else
		{
			// let's add an item
			ItemPtr new_item = std::make_shared<Item>(word, false, callbackID);
			node->items.push_back(new_item);
		}
	}

	if (m_renderer)
		display(nullptr);
}

ItemPtr DropDownMenu::getChildItem(const std::string& label, ItemPtr parent) const
{
	ItemPtr result = nullptr;
	if (!parent)
		parent = m_items;
	if (parent && parent->isSubMenu)
	{
		for (ItemPtr child : parent->items)
		{
			if (child->label == label)
				result = child;
		}
	}
	return result;
}

void DropDownMenu::display(ItemPtr menu, int level, bool isMenuChanged)
{
	if (menu == nullptr)
		menu = m_items;
	if (!menu)
		return;
	if (m_cascading && level == 0)
	{
		if (auto renderer = dynamic_cast<DropDownMenuRenderer*>(m_renderer))
			renderer->clearItems();
		m_objectList.clear();
		m_visibleItems.clear();
		displayColumn(menu, m_cascadeOrigin);
		if (!m_objectList.empty())
		{
			m_pos = m_objectList.front()->getPos();
			point2D bottomRight = m_pos;
			for (const auto& button : m_objectList)
			{
				const auto shape = button->getShape();
				m_pos.x = std::min(m_pos.x, shape.position.x);
				m_pos.y = std::min(m_pos.y, shape.position.y);
				bottomRight.x = std::max(bottomRight.x, shape.position.x + shape.size.w);
				bottomRight.y = std::max(bottomRight.y, shape.position.y + shape.size.h);
			}
			m_size = size2D(bottomRight.x - m_pos.x, bottomRight.y - m_pos.y);
		}
		updateRendererMenu();
		return;
	}
	if (level == 0)
		m_objectList.clear();
	// let's create the buttons of a menu. It is iterative.
	// Level will help us to know which subLevel the menu is. 
	std::string padding = "";
	for (int i = 0; i < level; i++)
		padding += "   ";

	for (auto item : menu->items)
	{
		if (item->isSubMenu)
		{
			addSubMenuButton(padding + (item->isOpen?"v":">") + item->label, item);
			if (item->isOpen)
			{
				// We create a button open menu
				display(item, level + 1);
			}
		}
		else
		{
			// We create a simple button with its callback
			addItemButton(padding + item->label, item->callbackID);
		}
	}

	if (isMenuChanged)
		updateRendererMenu();
	if (level == 0)
		updateRenderer();
}

void DropDownMenu::displayColumn(ItemPtr menu, point2D position, float parentWidth)
{
	float width = m_itemSize.w;
	for (const auto& item : menu->items)
	{
		int textWidth = 0;
		int textHeight = 0;
		const auto label = item->label + (item->isSubMenu ? " >" : "");
		if (m_font && m_font->isGenerated()
			&& TTF_GetStringSize(m_font->toSDL(), label.c_str(), 0, &textWidth, &textHeight))
			width = std::max(width, static_cast<float>(textWidth + 20));
	}
	int outputWidth = 0;
	int outputHeight = 0;
	size_t visibleCount = menu->items.size();
	if (sdl_renderer && SDL_GetRenderOutputSize(sdl_renderer, &outputWidth, &outputHeight))
	{
		width = std::min(width, static_cast<float>(outputWidth));
		visibleCount = std::min(visibleCount, std::max(size_t(1), static_cast<size_t>(outputHeight / m_itemSize.h)));
		if (parentWidth > 0)
			position.x = position.x + parentWidth + width <= outputWidth
				? position.x + parentWidth : position.x - width;
		position.x = std::clamp(position.x, 0.0f, std::max(0.0f, outputWidth - width));
		position.y = std::clamp(position.y, 0.0f,
			std::max(0.0f, outputHeight - static_cast<float>(visibleCount) * m_itemSize.h));
	}
	else
		position.x += parentWidth;
	menu->firstVisibleItem = std::min(menu->firstVisibleItem, menu->items.size() - visibleCount);
	ItemPtr openMenu;
	point2D openPosition;
	for (size_t index = menu->firstVisibleItem; index < menu->firstVisibleItem + visibleCount; ++index)
	{
		const auto& item = menu->items[index];
		addItemButton(item->label + (item->isSubMenu ? " >" : ""),
			item->isSubMenu ? "Nothing" : item->callbackID, size2D(width, m_itemSize.h));
		const auto button = m_objectList.back();
		button->setPos(position);
		m_visibleItems.emplace_back(button, item);
		if (item->isSubMenu && item->isOpen)
		{
			openMenu = item;
			openPosition = position;
		}
		position.y += m_itemSize.h;
	}
	if (openMenu)
		displayColumn(openMenu, openPosition, width);
}

void DropDownMenu::sortItems()
{
	if (m_items)
		sortItems(m_items);
}

void DropDownMenu::sortItems(ItemPtr menu)
{
	std::stable_sort(menu->items.begin(), menu->items.end(), [](const ItemPtr& first, const ItemPtr& second) {
		if (first->isSubMenu != second->isSubMenu)
			return first->isSubMenu;
		return std::lexicographical_compare(first->label.begin(), first->label.end(),
			second->label.begin(), second->label.end(), [](unsigned char firstChar, unsigned char secondChar) {
				return std::tolower(firstChar) < std::tolower(secondChar);
			});
	});
	for (const auto& item : menu->items)
		if (item->isSubMenu)
			sortItems(item);
}

void DropDownMenu::closeSubMenus(ItemPtr menu)
{
	for (const auto& item : menu->items)
	{
		if (item->isSubMenu)
		{
			item->isOpen = false;
			closeSubMenus(item);
		}
	}
}

void DropDownMenu::printMenu(ItemPtr menu, int level)
{
	if (menu == nullptr)
		menu = m_items;
	// let's create the buttons of a menu. It is iterative.
	// Level will help us to know which subLevel the menu is. 
	std::string padding = std::string(level, ' ');

	for (const auto& item : menu->items)
	{
		if (item->isSubMenu)
		{
			std::cout << padding + item->label << std::endl;
			if (item->isOpen)
			{
				// We create a button open menu
				printMenu(item, level + 1);
			}
		}
		else
		{
			// We create a simple button with its callback
			std::cout << padding + item->label << std::endl;
		}
	}
}

void DropDownMenu::updateRenderer()
{
	if (m_renderer)
	{
		static_cast<DropDownMenuRenderer*>(m_renderer)->update();
	}
}

void DropDownMenu::hover(point2D pos)
{
	m_isHovered = true;
	ObjectPtr hoveredButton;
	if (m_cascading)
	{
		ItemPtr hovered;
		for (const auto& entry : m_visibleItems)
			if (entry.first->getShape().isInside(pos))
			{
				hovered = entry.second;
				hoveredButton = entry.first;
			}
		if (!hovered)
		{
			leftFocus(pos);
			return;
		}
		ItemPtr parent;
		std::function<void(ItemPtr)> findParent = [&](ItemPtr node) {
			for (const auto& child : node->items)
			{
				if (child == hovered)
					parent = node;
				else if (child->isSubMenu)
					findParent(child);
			}
		};
		findParent(m_items);
		m_hoveredMenu = parent;
		bool changed = false;
		for (const auto& sibling : parent->items)
		{
			if (sibling != hovered && sibling->isSubMenu && sibling->isOpen)
			{
				sibling->isOpen = false;
				closeSubMenus(sibling);
				changed = true;
			}
		}
		if (hovered->isSubMenu && !hovered->isOpen)
		{
			hovered->isOpen = true;
			changed = true;
		}
		if (changed)
		{
			display(nullptr, 0, true);
			for (const auto& entry : m_visibleItems)
				if (entry.second == hovered)
					hoveredButton = entry.first;
		}
	}
	// let's find which button is hover
	// And all other button have left focus
	rect2D mouseRect = rect2D(pos, size2D(5, 5));
	int i = 0;
	for (const auto& item : m_objectList)
	{
		bool isHovered = m_cascading ? item == hoveredButton : mouseRect.isCollide(item->getShape());

		if (isHovered)
		{
			item->hover(pos);
		}
		else if (!isHovered)
		{
			item->leftFocus(pos);
		}
	}
	updateRenderer();
}

void DropDownMenu::leftFocus(point2D pos)
{
	for (auto item : m_objectList)
	{
		item->leftFocus(pos);
	}
	updateRenderer();
}

bool DropDownMenu::leftClick(point2D pos)
{
	// let's seek which button is clicked on
	rect2D mouseClick = rect2D(pos, size2D(5, 5));
	ObjectPtr clickedButton;
	if (m_cascading)
		for (const auto& item : m_objectList)
			if (item->getShape().isInside(pos))
				clickedButton = item;

	for (auto item : m_objectList)
	{
		bool wasLeftClick = item->isLeftClicked();
		bool isLeftClick = m_cascading ? item == clickedButton : mouseClick.isCollide(item->getShape());

		if (!wasLeftClick && isLeftClick)
		{
			item->onLeftClick(pos);
		}
		else if (wasLeftClick && !isLeftClick)
		{
			item->offLeftClick(pos);
		}
	}
	return true;
}

bool DropDownMenu::releaseLeftClick(point2D pos)
{
	m_isLeftClicked = false;
	for (auto item : m_objectList)
	{
		if (item->isLeftClicked())
		{
			item->offLeftClick(pos);
			return true;
		}
	}
	return true;
}

bool DropDownMenu::scroll(float dz)
{
	if (m_cascading && m_hoveredMenu && dz != 0)
	{
		int outputWidth = 0;
		int outputHeight = 0;
		if (!sdl_renderer || !SDL_GetRenderOutputSize(sdl_renderer, &outputWidth, &outputHeight))
			return true;
		const auto visibleCount = std::min(m_hoveredMenu->items.size(),
			std::max(size_t(1), static_cast<size_t>(outputHeight / m_itemSize.h)));
		const auto previous = m_hoveredMenu->firstVisibleItem;
		if (dz > 0 && previous > 0)
			--m_hoveredMenu->firstVisibleItem;
		else if (dz < 0 && previous < m_hoveredMenu->items.size() - visibleCount)
			++m_hoveredMenu->firstVisibleItem;
		if (previous != m_hoveredMenu->firstVisibleItem)
		{
			closeSubMenus(m_hoveredMenu);
			display(nullptr, 0, true);
		}
		return true;
	}
	std::cout << "scrolling on drop down menu with " << dz << std::endl;
	return true;
}
