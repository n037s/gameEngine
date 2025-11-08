// main start

#include "Viewer.h"
#include "CommandProxy.h"
#include "Controls.h"

#include "World.h"
#include "Sprite.h"

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

    Sprite* img = new Sprite({ 0,0 }, "boule");
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