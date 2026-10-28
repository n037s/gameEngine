#include "CommandProxy.h"

CommandProxy::CommandProxy()
{
	m_callbacks = std::map<SDL_Keycode, callbackFunction>();
	m_runningCallbacks = std::list<SDL_Keycode>();
}

const bool CommandProxy::bindCallback(const SDL_Keycode keycode, const callbackFunction& callback, const callbackFunction& onRelease)
{
	bool success = false;

	if (m_callbacks.find(keycode) == m_callbacks.end()) // no callback already exists
	{
		m_callbacks[keycode] = callback;
		if (onRelease != nullptr)
		{
			m_onReleaseCallbacks[keycode] = onRelease;
		}
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
	if (m_onReleaseCallbacks.find(keycode) != m_onReleaseCallbacks.end()) // the callback exists
	{
		m_onReleaseCallbacks.erase(keycode);
	}
	
	return success;
}

const bool CommandProxy::clearCallbacks()
{
	bool success = true;

	m_callbacks = std::map<SDL_Keycode, callbackFunction>();
	m_onReleaseCallbacks = std::map<SDL_Keycode, callbackFunction>();

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
		auto onReleaseCallback = m_onReleaseCallbacks.find(keycode);
		if (onReleaseCallback != m_onReleaseCallbacks.end()) // callback is found
		{
			(onReleaseCallback->second)();
		}
	}
}