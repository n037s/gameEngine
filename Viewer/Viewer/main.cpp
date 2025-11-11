// main start

#include "Viewer.h"
#include "CommandProxy.h"
#include "Controls.h"

#include "Geometry.h"

#include "World.h"
#include "Sprite.h"
#include "Text.h"
#include "Button.h"
#include "Font.h"
#include "FrameRateText.h"
#include "AnimatedSprite.h"

#include <iostream>

Sprite* img, *img2, *img3, *img4;

bool Osef(point2D pos)
{
    img->setIsHidden(!img->isHidden());
    img2->setIsHidden(!img2->isHidden());
    img4->setIsHidden(!img4->isHidden());
    std::cout << "my function click done at position : " << pos.toString() << std::endl;
    return true;
}

bool Osef2(point2D pos)
{
    std::cout << "my function click released with position : " << pos.toString() << std::endl;
    return true;
}

int main(int argc, char* argv[]) {
	// Define a viewer
	size2D windowSize{ 1280, 800 };

	Viewer* viewer = Viewer::getInstance();
    viewer->Init(windowSize);

    auto renderer = viewer->getRenderer();
	
	// Define a world
    World world;
    viewer->setWorld(&world);

	// Define its objects
    world.createCamera({ 0,0 }, windowSize);

    size2D objSize = size2D(256, 256);

    point2D r_pos = { 0,0 };
    img = new Sprite(r_pos * objSize, "craftpix/Tile/Top-Down Simple Summer_Ground 01");
    img->createRenderer(renderer);
    world.addObject(img);

    r_pos = { 1,0 };
    img2 = new Sprite(r_pos * objSize, "craftpix/Tile/Top-Down Simple Summer_Ground 02");
    img2->createRenderer(renderer);
    world.addObject(img2);

    r_pos = { 2,0 };
    img3 = new Sprite(r_pos * objSize, "craftpix/Tile/Top-Down Simple Summer_Ground 03");
    img3->createRenderer(renderer);
    world.addObject(img3);

    r_pos = { 0,1 };
    img4 = new Sprite(r_pos * objSize, "craftpix/Tile/Top-Down Simple Summer_Ground 04");
    img4->createRenderer(renderer);
    world.addObject(img4);

    for (int i = 5; i < 10; i++)
    {
        r_pos = point2D((i - 1) % 3, (i - 1) / 3);
        std::cout << "rpos " << r_pos.toString() << " id : " << "craftpix/Tile/Top-Down Simple Summer_Ground 0" + std::to_string(i) << std::endl;
        Sprite* img5 = new Sprite(r_pos * objSize, "craftpix/Tile/Top-Down Simple Summer_Ground 0"+std::to_string(i));
        img5->createRenderer(renderer);
        world.addObject(img5);
    }
    
    rect2D textPosition = rect2D(0, 0, 100, 30);
    FrameRateText* text = new FrameRateText(textPosition, "arial", 30, { 255, 255, 0, 255 });
    text->createRenderer(renderer);
    world.addOverlayObject(text);

    rect2D buttonPosition = rect2D(-50, -100, 100, 25);
    Font* font = new Font("arial", 25);
    SDL_Color color = { 100, 100, 100, 255 };
    SDL_Color fontColor = { 255, 255, 255, 255 };
    Button* button = new Button(buttonPosition, "blip", color, font, fontColor, Osef, Osef2);
    button->createRenderer(renderer);
    world.addObject(button);

    std::string basename = "FarmRPG/Character/";
    std::vector<std::string> persoAnimation = {
        basename + "Idle_front1",
        basename + "Idle_front2",
        basename + "Idle_front3",
        basename + "Idle_front4"
    };
    auto personnage = new AnimatedSprite(point2D(0,0), persoAnimation, 250);
    personnage->createRenderer(renderer);
    world.addObject(personnage);

	// Define controller manager and its callbacks
    CommandProxy cmdProxy;
    viewer->setCommandProxy(&cmdProxy);

    cmdProxy.bindCallback(SDLK_UP, CameraControl::moveUp);
    cmdProxy.bindCallback(SDLK_DOWN, CameraControl::moveDown);
    cmdProxy.bindCallback(SDLK_RIGHT, CameraControl::moveRight);
    cmdProxy.bindCallback(SDLK_LEFT, CameraControl::moveLeft);
    cmdProxy.bindCallback(SDLK_ESCAPE, Viewer::stopLoop);

	// Run application loop
    viewer->startLoop();

    return 0;
}


// Then What to do : 
// Images must be scaled to follow camera : SDL_BlitSurfaceScaled
// - Worl Manager (saving and reading in a file)
// - Start a project world editor. It needs to add the right click option to add a menu interfacing AssetsManager to create item
// + items can be grabbed and moved and resized.