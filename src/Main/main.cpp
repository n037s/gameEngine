// main start

#include "Viewer.h"
#include "CommandProxy.h"
#include "Controls.h"

#include "World.h"
#include "ObjectManager.h"
#include "ButtonCallbackRegistery.h"
#include "ObjectMemberHolder.h"
#include "AssetsManager.h"
#include "Player.h"

#include <iostream>
#include <sstream>

bool Osef2(point2D pos, Object* caller)
{
    const std::string objectUID = "e9337627-78e6-4df5-809c-544d88884ac3";

    std::cout << "osef2 called" << std::endl;
    World* world = Viewer::getInstance()->getWorld();
    if (!world)
    {
        std::cerr << "Cannot toggle object visibility: no world is loaded." << std::endl;
        return false;
    }
    auto object = world->getObjectByUID(objectUID);
    if (!object)
    {
        std::cerr << "Cannot toggle object visibility: target object " << objectUID << " was not found." << std::endl;
        return false;
    }
    object->setIsHidden(!object->isHidden());
    return true;
}

void createPlayer(World& world, SDL_Renderer* renderer)
{
    auto playerSprite = std::dynamic_pointer_cast<AnimatedSprite>(world.getObjectByTypeName("AnimatedSprite"));
    if (!playerSprite)
    {
        playerSprite = std::make_shared<AnimatedSprite>(point2D(0, 0), true, 250);
        playerSprite->setAnimation("FarmRPG/Character/Idle_front");
        playerSprite->createRenderer(renderer);
        playerSprite->setZ(1);
        world.addObject(playerSprite);
    }
    Player::getInstance()->setSprite(playerSprite);
}

int main(int argc, char* argv[]) 
{
    const std::string worldFilePath = "Configs/world";

    // Define a viewer
    const size2D windowSize{ 1280, 800 };

    Viewer* viewer = Viewer::getInstance();
    viewer->Init(windowSize);

    auto renderer = viewer->getRenderer();

    // Register all callbacks
    ButtonCallbackRegistery* btnRegister = ButtonCallbackRegistery::instance();
    btnRegister->registerCallback("osef2", Osef2);

    // Define a world
    World world;
    viewer->setWorld(&world);

    world.parseFile(worldFilePath);

    world.createRenderers(renderer);

    // Define its objects
    auto camera = world.createCamera({ 0,0 }, windowSize);
    world.setCameraZLimits(0.1f, 5);
    camera->setViewMode(Camera::ViewMode::Isometric);

    createPlayer(world, renderer);

    const size2D frameRateSize = size2D(100, 30);
    const rect2D textPosition = rect2D(point2D(windowSize.w - frameRateSize.w, 0), frameRateSize);
    const std::shared_ptr<FrameRateText> text = std::make_shared<FrameRateText>(textPosition, "arial", 30, Color(255, 255, 0));
    text->createRenderer(renderer);
    world.addOverlayObject(text);

    // Define controller manager and its callbacks
    CommandProxy cmdProxy;
    viewer->setCommandProxy(&cmdProxy);

    // All this must be deleted, it will move the player
    cmdProxy.bindCallback(SDLK_UP, &PlayerControl::moveUp, &PlayerControl::stopMoving);
    cmdProxy.bindCallback(SDLK_DOWN, &PlayerControl::moveDown, &PlayerControl::stopMoving);
    cmdProxy.bindCallback(SDLK_RIGHT, &PlayerControl::moveRight, &PlayerControl::stopMoving);
    cmdProxy.bindCallback(SDLK_LEFT, &PlayerControl::moveLeft, &PlayerControl::stopMoving);
    cmdProxy.bindCallback(SDLK_ESCAPE, Viewer::stopLoop);

    // Run application loop
    viewer->startLoop();

    // After the loop
    world.saveFile(worldFilePath);

    return 0;
}


// Then What to do : 
// - Start a project world editor. It needs to add the right click option to add a menu interfacing AssetsManager to create item
// + Drop down items : still needs to have scrollability + Fix the non writing in the world of this item.
// + Bounding Box items. Parasite to an object and controls its size and position.
// + items can be grabbed and moved and resized.
// - issue on dependencies. WorldEditor should not be dependent of SDL3 ... caused by import on .h