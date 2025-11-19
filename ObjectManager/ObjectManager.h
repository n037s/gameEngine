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

using objectDeserializer = std::function<std::shared_ptr<Object>(ObjectMemberHolder members)>;

class ObjectManager
{
public:
    static void registerObject(const std::string& name, objectDeserializer objectDeserializer) {
        getRegistry()[name] = std::move(objectDeserializer);
    }

    static std::shared_ptr<Object> createObject(const std::string& name, ObjectMemberHolder members) {
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
static ObjectRegister AnimatedSpriteReg("AnimatedSprite", [](ObjectMemberHolder members) { return std::make_shared<AnimatedSprite>(members); });
static ObjectRegister ButtonReg("Button", [](ObjectMemberHolder members) { return std::make_shared<Button>(members); });
static ObjectRegister FrameRateTextReg("FrameRateText", [](ObjectMemberHolder members) { return std::make_shared<FrameRateText>(members); });
static ObjectRegister SpriteReg("Sprite", [](ObjectMemberHolder members) { return std::make_shared<Sprite>(members); });
static ObjectRegister TextReg("Text", [](ObjectMemberHolder members) { return std::make_shared<Text>(members); });