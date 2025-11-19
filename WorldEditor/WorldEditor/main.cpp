// main start

#include "Viewer.h"
#include "CommandProxy.h"
#include "Controls.h"

#include "Geometry.h"
#include "Color.h"

#include "World.h"
#include "Sprite.h"
#include "Text.h"
#include "Button.h"
#include "Font.h"
#include "FrameRateText.h"
#include "AnimatedSprite.h"

#include "ButtonCallbackRegistery.h"

#include "ObjectMemberHolder.h"

#include <iostream>

bool Osef(point2D pos, Object* caller)
{
    return true;
}

bool Osef2(point2D pos, Object* caller)
{
    std::cout << "osef2 called" << std::endl;
    return true;
}

void CreateMyObjects(SDL_Renderer* renderer, World& world)
{
    size2D objSize = size2D(256, 256);
    std::cout << "Creating the world ..." << std::endl;

    point2D r_pos = { 0,0 };
    std::shared_ptr<Sprite> img = std::make_shared<Sprite>(r_pos * objSize, "craftpix/Tile/Top-Down Simple Summer_Ground 01");
    img->createRenderer(renderer);
    world.addObject(img);

    r_pos = { 1,0 };
    std::shared_ptr<Sprite> img2 = std::make_shared<Sprite>(r_pos * objSize, "craftpix/Tile/Top-Down Simple Summer_Ground 02");
    img2->createRenderer(renderer);
    world.addObject(img2);

    r_pos = { 2,0 };
    std::shared_ptr<Sprite> img3 = std::make_shared<Sprite>(r_pos * objSize, "craftpix/Tile/Top-Down Simple Summer_Ground 03");
    img3->createRenderer(renderer);
    world.addObject(img3);

    r_pos = { 0,1 };
    std::shared_ptr<Sprite> img4 = std::make_shared<Sprite>(r_pos * objSize, "craftpix/Tile/Top-Down Simple Summer_Ground 04");
    img4->createRenderer(renderer);
    world.addObject(img4);

    for (int i = 5; i < 10; i++)
    {
        r_pos = point2D((i - 1) % 3, (i - 1) / 3);
        std::cout << "rpos " << r_pos.toString() << " id : " << "craftpix/Tile/Top-Down Simple Summer_Ground 0" + std::to_string(i) << std::endl;
        std::shared_ptr<Sprite> img5 = std::make_shared<Sprite>(r_pos * objSize, "craftpix/Tile/Top-Down Simple Summer_Ground 0" + std::to_string(i));
        img5->createRenderer(renderer);
        world.addObject(img5);
    }

    rect2D textPosition = rect2D(0, 0, 100, 30);
    std::shared_ptr<FrameRateText> text = std::make_shared<FrameRateText>(textPosition, "arial", 30, Color(255, 255, 0));
    text->createRenderer(renderer);
    world.addOverlayObject(text);

    rect2D buttonPosition = rect2D(0, -100, 100, 25);
    Font* font = new Font("arial", 25);
    Color color = Color(100, 100, 100, 255);
    Color fontColor = Color(255, 255, 255, 255);
    std::shared_ptr<Button> button = std::make_shared<Button>(buttonPosition, "blip", color, font, fontColor, "osef", "osef2");
    button->setZ(3);
    button->createRenderer(renderer);
    world.addObject(button);

    std::string basename = "FarmRPG/Character/";
    std::vector<std::string> persoAnimation = {
        basename + "Idle_front1",
        basename + "Idle_front2",
        basename + "Idle_front3",
        basename + "Idle_front4"
    };
    auto personnage = std::make_shared<AnimatedSprite>(point2D(0, 0), persoAnimation, 250);
    personnage->createRenderer(renderer);
    personnage->setZ(1);
    world.addObject(personnage);
}

int main(int argc, char* argv[]) 
{

    std::string worldFilePath = "C:\\Users\\leoqu\\Desktop\\Code\\Project\\Configs\\world";

    // Define a viewer
    size2D windowSize{ 1280, 800 };

    Viewer* viewer = Viewer::getInstance();
    viewer->Init(windowSize);

    auto renderer = viewer->getRenderer();

    // Register all callbacks
    ButtonCallbackRegistery* btnRegister = ButtonCallbackRegistery::instance();
    btnRegister->registerCallback("osef", Osef);
    btnRegister->registerCallback("osef2", Osef2);

    // Define a world
    World world;
    viewer->setWorld(&world);

    world.parseFile(worldFilePath);
    world.createRenderers(renderer);

    // Define its objects
    world.createCamera({ 0,0 }, windowSize);
    world.setCameraZLimits(0.2f, 5);

    // Create world
    // CreateMyObjects(renderer, world);

    // Define controller manager and its callbacks
    CommandProxy cmdProxy;
    viewer->setCommandProxy(&cmdProxy);

    cmdProxy.bindCallback(SDLK_UP, CameraControl::moveUp);
    cmdProxy.bindCallback(SDLK_DOWN, CameraControl::moveDown);
    cmdProxy.bindCallback(SDLK_RIGHT, CameraControl::moveRight);
    cmdProxy.bindCallback(SDLK_LEFT, CameraControl::moveLeft);
    cmdProxy.bindCallback(SDLK_ESCAPE, Viewer::stopLoop);

    viewer->setWhellCallback(CameraControl::moveZ);

    // Run application loop
    viewer->startLoop();

    // After the loop
    // world.saveFile(worldFilePath);

    return 0;
}


// Then What to do : 
// - Worl Manager (saving and reading in a file)
// - Start a project world editor. It needs to add the right click option to add a menu interfacing AssetsManager to create item
// + items can be grabbed and moved and resized.
// - issue on dependencies. WorldEditor should not be dependent of SDL3 ... caused by import on .h