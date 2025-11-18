#include "ObjectManager.h"

// ===== Objects Registration =====
static ObjectRegister AnimatedSpriteReg("AnimatedSprite", [](ObjectMemberHolder members) { return std::make_shared<AnimatedSprite>(members); });
static ObjectRegister ButtonReg("Button", [](ObjectMemberHolder members) { return std::make_shared<Button>(members); });
static ObjectRegister FrameRateTextReg("FrameRateText", [](ObjectMemberHolder members) { return std::make_shared<FrameRateText>(members); });
static ObjectRegister SpriteReg("Sprite", [](ObjectMemberHolder members) { return std::make_shared<Sprite>(members); });
static ObjectRegister TextReg("Text", [](ObjectMemberHolder members) { return std::make_shared<Text>(members); });