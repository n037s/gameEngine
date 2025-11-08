#include "CommandProxy.h"

CommandProxy::CommandProxy()
{
	m_callbacks = std::map<SDL_Keycode, callbackFunction>();
	m_runningCallbacks = std::list<SDL_Keycode>();
}

const bool CommandProxy::bindCallback(const SDL_Keycode keycode, callbackFunction callback)
{
	bool success = false;

	if (m_callbacks.find(keycode) == m_callbacks.end()) // no callback already exists
	{
		m_callbacks[keycode] = callback;
		success = true;
	}

	return success;
}

const bool CommandProxy::unbindCallback(const SDL_Keycode keycode)
{
	bool success = false;

	if (m_callbacks.find(keycode) != m_callbacks.end()) // the callback exists
	{
		m_callbacks.erase(keycode);
		success = true;
	}
	
	return success;
}

const bool CommandProxy::clearCallbacks()
{
	bool success = true;

	m_callbacks = std::map<SDL_Keycode, callbackFunction>();

	return success;
}

const bool CommandProxy::runCallback(const SDL_Keycode keycode) const
{
	bool success = false;

	auto callback = m_callbacks.find(keycode);
	if (callback != m_callbacks.end()) // callback is found
	{
		success = (callback->second)();
	}

	return success;
}

const void CommandProxy::runCallbacks() const
{
	for (SDL_Keycode keycode : m_runningCallbacks)
	{
		runCallback(keycode);
	}
}

bool CommandProxy::addRunningCallback(const SDL_Keycode keycode)
{
	bool success = false;
	auto keycodeIterator = std::find(m_runningCallbacks.begin(), m_runningCallbacks.end(), keycode);
	if (keycodeIterator == m_runningCallbacks.end())
	{
		m_runningCallbacks.push_back(keycode);
		success = true;
	}
	return success;
}

void CommandProxy::removeRunningCallback(const SDL_Keycode keycode)
{
	auto keycodeIterator = std::find(m_runningCallbacks.begin(), m_runningCallbacks.end(), keycode);
	if (keycodeIterator != m_runningCallbacks.end())
	{
		m_runningCallbacks.erase(keycodeIterator);
	}
}