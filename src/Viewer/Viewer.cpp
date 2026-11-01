// Viewer.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//
#include "Viewer.h"

#include <string>

#include "SDL_ttf.h"

#include "CommandProxy.h"
#include "FrameRate.h"

Viewer* Viewer::m_instance = nullptr;
bool Viewer::m_running = false;

bool Viewer::Init(size2D windowSize)
{
    m_logger = Logger::getInstance("Viewer");
    m_globalConfig = &GlobalConfig::getInstance();

    m_windowSize = windowSize;
    SDL_Init(SDL_INIT_VIDEO);

    m_logger->info("Viewer::Init() - Appication basepath : " + std::string(SDL_GetBasePath()));

    m_window = SDL_CreateWindow("MyApp", (int)m_windowSize.w, (int)m_windowSize.h, 0);
    if (m_window == nullptr) {
        m_logger->error("SDL_CreateWindow Error: " + std::string(SDL_GetError()));
        SDL_Quit();
        isFailure = true;
    }
    else
    {
        m_renderer = SDL_CreateRenderer(m_window, NULL);
        if (m_renderer == nullptr) {
            m_logger->error("SDL_CreateRenderer Error: " + std::string(SDL_GetError()));
            SDL_DestroyWindow(m_window);
            SDL_Quit();
            isFailure = true;
        }
    }

    if (!TTF_Init()) {
        m_logger->error("Couldn't initialize SDL_ttf: " + std::string(SDL_GetError()));
        return false; // SDL_APP_FAILURE;
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
    const Uint64 frameBudget = SDL_NS_PER_SECOND / m_globalConfig->getFrameRateLimit();
    FrameRate& frameRate = FrameRate::getInstance();
    frameRate.reset();
    m_running = true;
    while (m_running) {
        const Uint64 frameStart = SDL_GetTicksNS();
        frameRate.update();

        // Handle events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            manageUserInput(event);
        }

        m_commandProxy->runCallbacks();

        // Clear the screen
        SDL_SetRenderDrawColor(m_renderer, 0, 20, 70, 255);
        SDL_RenderClear(m_renderer);

        m_world->update();
        m_world->render(m_renderer);

        // Present the screen
        SDL_RenderPresent(m_renderer);

        // Cap the frame rate
        const Uint64 frameDuration = SDL_GetTicksNS() - frameStart;
        if (frameDuration < frameBudget)
            SDL_DelayNS(frameBudget - frameDuration);
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
        const SDL_Keycode keycode = event.key.key;

        m_logger->info("key pressed : " + std::string(SDL_GetKeyName(keycode)) + " returns : " +
            (m_commandProxy->addRunningCallback(keycode) ? "true" : "false"));
        
        break;
    }
    case SDL_EVENT_KEY_UP:
    {
        const SDL_Keycode keycode = event.key.key;
        m_logger->info("key released : " + std::string(SDL_GetKeyName(keycode)));
        m_commandProxy->removeRunningCallback(keycode);
        break;
    }
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    {
        const point2D clickpos = point2D(event.button.x, event.button.y);
        if (event.button.button == SDL_BUTTON_LEFT)
        {
            m_world->leftClick(clickpos);
        }
        else if (event.button.button == SDL_BUTTON_RIGHT)
        {
            m_world->rightClick(clickpos);
        }
        break;
    }
    case SDL_EVENT_MOUSE_BUTTON_UP:
    {
        const point2D clickpos = point2D(event.button.x, event.button.y);
        if (event.button.button == SDL_BUTTON_LEFT)
        {
            m_world->releaseLeftClick(clickpos);
        }
        else if (event.button.button == SDL_BUTTON_RIGHT)
        {
            m_world->releaseRightClick(clickpos);
        }
        break;
    }
    case SDL_EVENT_MOUSE_MOTION:
    {
        const point2D clickpos = point2D(event.motion.x, event.motion.y);
        m_world->mouseMove(clickpos);
        break;
    }
    case SDL_EVENT_MOUSE_WHEEL:
    {
        const float dy = event.wheel.y;
        if (m_wheelCallback)
        {
            m_wheelCallback(dy);
        }
        break;
    }
    }
    
}