#pragma once

#include "World.h"
#include "CommandProxy.h"

class Viewer
{
public:
	static Viewer* getInstance() {
		if (!m_instance)
			m_instance = new Viewer();
		return m_instance;
	}

	bool Init(size2D windowSize);

	void startLoop();
	static bool stopLoop();

	void manageUserInput(SDL_Event event);

	SDL_Window* getWindow() const { return m_window; }
	SDL_Renderer* getRenderer() const { return m_renderer; }

	void setWorld(World* world) { m_world = world; }
	World* getWorld() { return m_world; }
	void setCommandProxy(CommandProxy* cmdProxy) { m_commandProxy = cmdProxy; }

	void setWhellCallback(callbackMouseWheel callback) { m_wheelCallback = callback; }

	World* getWorld() const { return m_world; }

private:
	Viewer();
	~Viewer();
	static Viewer* m_instance;
	static bool m_running;

	bool isFailure{ false };
	size2D m_windowSize{ 0,0 };

	SDL_Window* m_window{ nullptr };
	SDL_Renderer* m_renderer{ nullptr };
	float m_SDLRenderingScale{ 1 };

	World* m_world{ nullptr };
	CommandProxy* m_commandProxy{ nullptr };

	callbackMouseWheel m_wheelCallback{ nullptr };
};