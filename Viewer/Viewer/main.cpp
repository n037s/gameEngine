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

#include <iostream>

bool Osef(point2D pos)
{
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

    Sprite* img = new Sprite({0,0}, "boule");
    img->createRenderer(renderer);
    world.addObject(img);

    Sprite* img2 = new Sprite({ 0,128 }, "boule");
    img2->createRenderer(renderer);
    world.addObject(img2);

    Sprite* img3 = new Sprite({ 0,256 }, "boule");
    img3->createRenderer(renderer);
    world.addObject(img3);

    Sprite* img4 = new Sprite({ 128,0 }, "boule");
    img4->createRenderer(renderer);
    world.addObject(img4);

    rect2D textPosition = rect2D(-200, 0, 100, 30);
    Text* text = new Text(textPosition, "test", "arial.ttf", 30, { 255, 255, 0, 255 });
    text->createRenderer(renderer);
    world.addObject(text);

    rect2D buttonPosition = rect2D(-50, -50, 100, 25);
    Font* font = new Font("C:\\Users\\leoqu\\Desktop\\Code\\Project\\Assets\\arial.ttf", 25);
    SDL_Color color = { 100, 100, 100, 255 };
    SDL_Color fontColor = { 255, 255, 255, 255 };
    Button* button = new Button(buttonPosition, "blip", color, font, fontColor, Osef, Osef2);
    button->createRenderer(renderer);
    world.addObject(button);

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