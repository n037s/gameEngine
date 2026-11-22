#include "CommandProxy.h"

#include <algorithm>
#include <string>

CommandProxy::CommandProxy() : m_logger(Logger::getInstance(Components::Viewer))
{
}

SDL_Keymod CommandProxy::normalizeModifiers(SDL_Keymod modifiers)
{
	SDL_Keymod normalized = SDL_KMOD_NONE;
	for (SDL_Keymod group : { SDL_KMOD_CTRL, SDL_KMOD_ALT, SDL_KMOD_SHIFT, SDL_KMOD_GUI })
	{
		if (modifiers & group)
			normalized |= group;
	}
	return normalized;
}

const bool CommandProxy::bindCallback(const Key keycode, const callbackFunction& callback, const callbackFunction& onRelease)
{
	const Key binding = Key::fromSDL(keycode.getKeycode(), normalizeModifiers(keycode.getModifierMask()));
	if (!m_callbacks.emplace(binding, callback).second)
	{
		m_logger->error("Callback already exists for keycode " + std::to_string(keycode.getKeycode()) +
			" with modifiers " + std::to_string(keycode.getModifierMask()));
		return false;
	}
	if (onRelease)
		m_onReleaseCallbacks.emplace(binding, onRelease);
	return true;
}

const bool CommandProxy::unbindCallback(const Key keycode)
{
	const Key binding = Key::fromSDL(keycode.getKeycode(), normalizeModifiers(keycode.getModifierMask()));
	m_onReleaseCallbacks.erase(binding);
	m_runningCallbacks.remove(binding);
	return m_callbacks.erase(binding) != 0;
}

const bool CommandProxy::clearCallbacks()
{
	bool success = true;

	m_callbacks.clear();
	m_onReleaseCallbacks.clear();
	m_pressedKeys.clear();
	m_runningCallbacks.clear();

	return success;
}

CommandProxy::CallbackMap::const_iterator CommandProxy::findCallback(const Key keycode) const
{
	const SDL_Keymod normalized = normalizeModifiers(keycode.getModifierMask());
	const auto callback = m_callbacks.find(Key::fromSDL(keycode.getKeycode(), normalized));
	if (callback != m_callbacks.end() || normalized == SDL_KMOD_NONE)
		return callback;
	return m_callbacks.find(Key(keycode.getKeycode()));
}

const bool CommandProxy::runCallback(const Key keycode) const
{
	const auto callback = findCallback(keycode);
	return callback != m_callbacks.end() && (callback->second)();
}

const void CommandProxy::runCallbacks() const
{
	const auto runningCallbacks = m_runningCallbacks;
	for (const Key& binding : runningCallbacks)
	{
		if (std::find(m_runningCallbacks.begin(), m_runningCallbacks.end(), binding) != m_runningCallbacks.end())
			runCallback(binding);
	}
}

bool CommandProxy::addRunningCallback(const Key keycode)
{
	const SDL_Keycode pressedKey = keycode.getKeycode();
	const bool added = std::find(m_pressedKeys.begin(), m_pressedKeys.end(), pressedKey) == m_pressedKeys.end();
	if (added)
		m_pressedKeys.push_back(pressedKey);
	updateRunningCallbacks(keycode.getModifierMask());
	return added;
}

void CommandProxy::removeRunningCallback(const Key keycode)
{
	m_pressedKeys.remove(keycode.getKeycode());
	updateRunningCallbacks(keycode.getModifierMask());
}

void CommandProxy::updateRunningCallbacks(SDL_Keymod modifiers)
{
	std::list<Key> nextCallbacks;
	for (SDL_Keycode keycode : m_pressedKeys)
	{
		SDL_Keymod keyModifiers = modifiers;
		switch (keycode)
		{
		case SDLK_LCTRL: case SDLK_RCTRL: keyModifiers &= ~SDL_KMOD_CTRL; break;
		case SDLK_LALT: case SDLK_RALT: keyModifiers &= ~SDL_KMOD_ALT; break;
		case SDLK_LSHIFT: case SDLK_RSHIFT: keyModifiers &= ~SDL_KMOD_SHIFT; break;
		case SDLK_LGUI: case SDLK_RGUI: keyModifiers &= ~SDL_KMOD_GUI; break;
		}
		const auto callback = findCallback(Key::fromSDL(keycode, keyModifiers));
		if (callback != m_callbacks.end())
			nextCallbacks.push_back(callback->first);
	}
	const auto previousCallbacks = m_runningCallbacks;
	m_runningCallbacks = nextCallbacks;
	for (const Key& binding : previousCallbacks)
	{
		if (std::find(nextCallbacks.begin(), nextCallbacks.end(), binding) == nextCallbacks.end())
		{
			auto onReleaseCallback = m_onReleaseCallbacks.find(binding);
			if (onReleaseCallback != m_onReleaseCallbacks.end())
			{
				const auto callback = onReleaseCallback->second;
				callback();
			}
		}
	}
}

void CommandProxy::releaseRunningCallbacks()
{
	m_pressedKeys.clear();
	updateRunningCallbacks(SDL_KMOD_NONE);
}