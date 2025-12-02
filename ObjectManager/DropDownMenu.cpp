#include "DropDownMenu.h"

#include "ButtonCallbackRegistery.h"
#include "Button.h"

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


void DropDownMenu::addItemButton(std::string label, std::string callbackID)
{
	int itemCount = static_cast<int>(m_objectList.size());
	size2D dy = m_itemSize * size2D(0, itemCount);
	point2D position = m_pos + dy;

	ObjectMemberHolder members;
	members.addMember("UID", UIDGenerator::generateUID());
	members.addMember("objectPosition", position);
	members.addMember("objectHeight", m_z);
	members.addMember("objectSize", m_itemSize);
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
	auto renderer = static_cast<DropDownMenuRenderer*>(m_renderer);
	renderer->clearItems();
	for (auto item : m_objectList)
	{
		item->createRenderer(sdl_renderer);
		renderer->addItem(static_cast<ButtonRenderer*>(item->getRenderer()));
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
		std::cout << "word : " << word << " is number " << current_word_iterator << "/" << ID_words_count << std::endl;
		if (current_word_iterator++ < ID_words_count)
		{
			// let's see if the subMenu exists
			ItemPtr child_node = getChildItem(word, node);
			if (!child_node)
			{
				std::cout << "add subMenu : " << word << std::endl;
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
			std::cout << "add item : " << word << std::endl;
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
	if (parent->isSubMenu)
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

void DropDownMenu::printMenu(ItemPtr menu, int level)
{
	if (menu == nullptr)
		menu = m_items;
	// let's create the buttons of a menu. It is iterative.
	// Level will help us to know which subLevel the menu is. 
	std::string padding = "";
	for (int i = 0; i < level; i++)
		padding += " ";

	for (auto item : menu->items)
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
	// let's find which button is hover
	// And all other button have left focus
	rect2D mouseRect = rect2D(pos, size2D(5, 5));
	int i = 0;
	for (auto item : m_objectList)
	{
		bool isHoovered = mouseRect.isCollide(item->getShape());

		if (isHoovered)
		{
			item->hover(pos);
		}
		else if (!isHoovered)
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

	for (auto item : m_objectList)
	{
		bool wasLeftClick = item->isLeftClicked();
		bool isLeftClick = mouseClick.isCollide(item->getShape());

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
