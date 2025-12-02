#pragma once

#include <string>
#include <map>
#include <functional>

#include "Object.h"

#include "AnimatedSprite.h"
#include "Button.h"
#include "FrameRateText.h"
#include "Sprite.h"
#include "Text.h"
#include "DropDownMenu.h"

using objectDeserializer = std::function<ObjectPtr(const ObjectMemberHolder& members)>;

class ObjectManager
{
public:
    static void registerObject(const std::string& name, objectDeserializer objectDeserializer) {
        getRegistry()[name] = std::move(objectDeserializer);
    }

    static ObjectPtr createObject(const std::string& name, const ObjectMemberHolder& members) {
        auto& registry = getRegistry();
        auto it = registry.find(name);
        if (it != registry.end()) {
            return (it->second)(members);
        }
        return nullptr; // Unknown item
    }

private:
    static std::unordered_map<std::string, objectDeserializer>& getRegistry() {
        static std::unordered_map<std::string, objectDeserializer> registry;
        return registry;
    }
};

struct ObjectRegister {
    ObjectRegister(const std::string& name, objectDeserializer objectDeserializer) {
        ObjectManager::registerObject(name, std::move(objectDeserializer));
    }
};

// ===== Objects Registration =====
static ObjectRegister AnimatedSpriteReg("AnimatedSprite", [](const ObjectMemberHolder& members) { return std::make_shared<AnimatedSprite>(members); });
static ObjectRegister ButtonReg("Button", [](const ObjectMemberHolder& members) { return std::make_shared<Button>(members); });
static ObjectRegister FrameRateTextReg("FrameRateText", [](const ObjectMemberHolder& members) { return std::make_shared<FrameRateText>(members); });
static ObjectRegister SpriteReg("Sprite", [](const ObjectMemberHolder& members) { return std::make_shared<Sprite>(members); });
static ObjectRegister TextReg("Text", [](const ObjectMemberHolder& members) { return std::make_shared<Text>(members); });