#pragma once

#include "SDL3/SDL.h"
#include <functional>
#include <map>
#include <list>
#include <utility>
#include "Key.h"
#include "Logger.h"

using callbackFunction = std::function<bool()>;
using callbackMouseWheel = std::function<bool(float d)>;

/*
* This class manages user events repartition
*/
class CommandProxy
{
public:
	/*
	 * @brief Constructor
	 */
	CommandProxy();

	/*
	 * @brief bind a new callback to a key pressed
	 * 
	 * @param keycode - SDL keycode to bind a command to
	 * @param callback - callback function to bind to the key
	 * @param modifiers - exact Ctrl/Alt/Shift/GUI combination; either side matches, lock keys are ignored
	 * 
	 * @returns boolean if the bind is successfull
	 */
	const bool bindCallback(const Key keycode, const callbackFunction& callback, const callbackFunction& onRelease = callbackFunction());

	template <typename Controller>
	const bool bindCallback(const Key keycode, bool (Controller::*callback)(), bool (Controller::*onRelease)() = nullptr)
	{
		callbackFunction releaseCallback;
		if (onRelease)
		{
			releaseCallback = [onRelease] {
				return (Controller::getInstance().*onRelease)();
			};
		}
		return bindCallback(keycode, 
			[callback] {
				return (Controller::getInstance().*callback)();
			},
			releaseCallback
		);
	}

	/*
	 * @brief Delete a callback to a SDL keycode
	 *
	 * @param keycode - SDL keycode to unbind a command to
	 * @param modifiers - modifier combination of the binding to remove
	 *
	 * @returns boolean if the unbind is successfull
	 */
	const bool unbindCallback(const Key keycode);

	/*
	 * @brief Delete all callbacks
	 *
	 * @returns boolean if the unbind is successfull
	 */
	const bool clearCallbacks();

	/*
	 * @brief run a callback by its keycode
	 *
	 * @param keycode - SDL keycode pressed
	 * @param modifiers - current SDL keyboard modifiers
	 *
	 * @returns boolean if the callback runned successfull
	 */
	const bool runCallback(const Key keycode) const;

	/*
	 * @brief run callbacks in m_runningCallbacks
	 *
	 * @returns boolean if the callback runned successfull
	 */
	const void runCallbacks() const;

	/*
	 * @brief add a callback into m_runningCallbacks
	 *
	 * @param keycode - SDL keycode pressed
	 * @param modifiers - current modifiers; held keys are re-evaluated when these change
	 */
	bool addRunningCallback(const Key keycode);

	/*
	 * @brief remove a callback into m_runningCallbacks
	 *
	 * @param keycode - SDL keycode pressed
	 * @param modifiers - modifiers remaining after the key release
	 */
	void removeRunningCallback(const Key keycode);

	/*
	 * @brief release all running callbacks
	 */
	void releaseRunningCallbacks();

private:
	using CallbackMap = std::map<Key, callbackFunction>;

	Logger* m_logger;
	CallbackMap m_callbacks;
	CallbackMap m_onReleaseCallbacks;
	std::list<SDL_Keycode> m_pressedKeys;
	std::list<Key> m_runningCallbacks;

	static SDL_Keymod normalizeModifiers(SDL_Keymod modifiers);
	CallbackMap::const_iterator findCallback(const Key keycode) const;
	void updateRunningCallbacks(SDL_Keymod modifiers);
};

// Not very happy with this, I would like to sort by SDL_Keycode, then by Keymod inside, so when pressing a key code
// we check if there is something with the key mode, if not, we do the key code originally. 

