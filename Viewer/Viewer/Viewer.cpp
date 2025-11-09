// Viewer.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//
#include "Viewer.h"

#include <iostream>
#include <string>

#include "SDL_ttf.h"

#include "CommandProxy.h"

const int fps = 20;

Viewer* Viewer::m_instance = nullptr;
bool Viewer::m_running = false;

bool Viewer::Init(size2D windowSize)
{
    m_windowSize = windowSize;
    SDL_Init(SDL_INIT_VIDEO);

    std::cout << "basepath : " << SDL_GetBasePath() << std::endl;

    m_window = SDL_CreateWindow("MyApp", (int)m_windowSize.w, (int)m_windowSize.h, 0);
    if (m_window == nullptr) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        isFailure = true;
    }
    else
    {
        m_renderer = SDL_CreateRenderer(m_window, NULL);
        if (m_renderer == nullptr) {
            std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(m_window);
            SDL_Quit();
            isFailure = true;
        }
    }

    if (!TTF_Init()) {
        SDL_Log("Couldn't initialize SDL_ttf: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    return isFailure;
}

Viewer::Viewer()
{

}

Viewer::~Viewer()
{
    // Clean up
    TTF_Quit();
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void Viewer::startLoop()
{
    int iii = 1;
    m_running = true;
    while (m_running) {
        std::cout << "[Application loop iteration]" << std::endl;

        // Handle events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            manageUserInput(event);
        }

        m_commandProxy->runCallbacks();

        // Clear the screen
        SDL_SetRenderDrawColor(m_renderer, 0, 0, 100, 255);
        SDL_RenderClear(m_renderer);

        m_world->render(m_renderer);

        // Present the screen
        SDL_RenderPresent(m_renderer);

        // Cap the frame rate
        SDL_Delay(1000 / fps);
    }
}

bool Viewer::stopLoop()
{
    m_running = false;
    return true;
}

void Viewer::manageUserInput(SDL_Event event)
{
    switch (event.type)
    {
    case SDL_EVENT_QUIT:
        m_running = false;
        break;
    case SDL_EVENT_KEY_DOWN:
    {
        SDL_Keycode keycode = event.key.key;

        std::cout << "key pressed : " << SDL_GetKeyName(keycode) << " returns : " <<
            (m_commandProxy->addRunningCallback(keycode) ? "true" : "false") << std::endl;
        
        break;
    }
    case SDL_EVENT_KEY_UP:
    {
        SDL_Keycode keycode = event.key.key;
        std::cout << "key released : " << SDL_GetKeyName(keycode) << std::endl;
        m_commandProxy->removeRunningCallback(keycode);
    }
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    {
        point2D clickpos = point2D(event.button.x, event.button.y);
        if (event.button.button == SDL_BUTTON_LEFT)
        {
            std::cout << "left click done " << clickpos.toString() << std::endl;
            // Need to define a class button that handle left click
            // When a button is left clicked, it modify its look partially and it call a callback defined in itself.
            // Warning : position of left click must be refered to the camera
        }
        else if (event.button.button == SDL_BUTTON_RIGHT)
        {
            std::cout << "right click done " << clickpos.toString() << std::endl;
            // Right click will for now make appear a contextual menu. So before let's handle left click
            // Then it will create a sort of list of button at the position of the right click.
        }
    }
    case SDL_EVENT_MOUSE_MOTION:
    {
        point2D clickpos = point2D(event.button.x, event.button.y);
        m_world->Hoovering(clickpos);
    }
    }
    
}
