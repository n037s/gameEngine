#include "ContextualObjectMenu.h"

#include "ButtonCallbackRegistery.h"
#include "World.h"
#include "AssetsManager.h"
#include "Sprite.h"
#include "AnimatedSprite.h"


ContextualObjectMenu::ContextualObjectMenu(ObjectPtr target, point2D position, World& world)
	: DropDownMenu(rect2D(position, size2D(180, 30)), size2D(180, 30),
		Color(180, 180, 180, 255), std::make_shared<Font>("arial", 20.0f)),
	  m_world(world), m_callbackID("ContextualObjectMenu_" + getUID())
{
    std::cout << "Creating ContextualObjectMenu" << std::endl;
	setSteady(true);
	setCascading(true);
	setZ(100001.0f);
	const bool steady = !target->isSteady();
	const std::weak_ptr<Object> weakTarget = target;
	ButtonCallbackRegistery::instance()->registerCallback(m_callbackID,
		[this, weakTarget, steady](point2D, Object*) {
			const auto object = weakTarget.lock();
			if (object)
				object->setSteady(steady);
			close();
			return object != nullptr;
		});
	addItem(steady ? "set Steady" : "unset Steady", m_callbackID);
	if (const auto sprite = std::dynamic_pointer_cast<Sprite>(target))
	{
		const std::weak_ptr<Sprite> weakSprite = sprite;
		const auto assets = AssetsManager::getInstance();
		for (const auto& assetID : assets->getAssets())
		{
			const auto callbackID = m_callbackID + "_asset_" + assetID;
			ButtonCallbackRegistery::instance()->registerCallback(callbackID,
				[this, weakSprite, assetID](point2D, Object*) {
					const auto sprite = weakSprite.lock();
					const bool updated = sprite && sprite->setAsset(assetID);
					close();
					return updated;
				});
			m_assetCallbackIDs.push_back(callbackID);
			const auto relativePath = fs::path(assets->getPath(assetID)).lexically_relative(assets->getAssetsPath());
			addItem("set asset/" + relativePath.generic_string(), callbackID);
		}
		sortItems();
	}
    if (const auto animation = std::dynamic_pointer_cast<AnimatedSprite>(target))
	{
        std::cout << "Creating animation menu" << std::endl;
		const std::weak_ptr<AnimatedSprite> weakAnimation = animation;
		const auto assets = AssetsManager::getInstance();
		for (const auto& assetID : assets->getAnimatedAssets())
		{
			const auto callbackID = m_callbackID + "_animation_" + assetID;
			ButtonCallbackRegistery::instance()->registerCallback(callbackID,
				[this, weakAnimation, assetID](point2D, Object*) {
					const auto animation = weakAnimation.lock();
					const bool updated = animation && animation->setAnimation(assetID);
					close();
					return updated;
				});
			m_assetCallbackIDs.push_back(callbackID);
			const auto relativePath = fs::path(assets->getPath(assetID)).lexically_relative(assets->getAssetsPath());
			addItem("set animation/" + relativePath.generic_string(), callbackID);
		}
		sortItems();
	}
	m_isHovered = true;
}

ContextualObjectMenu::~ContextualObjectMenu()
{
	ButtonCallbackRegistery::instance()->unregisterCallback(m_callbackID);
	for (const auto& callbackID : m_assetCallbackIDs)
		ButtonCallbackRegistery::instance()->unregisterCallback(callbackID);
}

void ContextualObjectMenu::leftFocus(point2D pos)
{
	DropDownMenu::leftFocus(pos);
	Object::leftFocus(pos);
}

void ContextualObjectMenu::outsideClick(point2D)
{
	close();
}

void ContextualObjectMenu::close()
{
	setIsHidden(true);
	const auto object = m_world.getObjectByUID(getUID());
	if (object)
		m_world.removeOverlayObject(object);
}