#pragma once

#include "SDL3/SDL.h"
#include <functional>
#include <map>
#include <list>

using callbackFunction = std::function<bool()>;

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
	 * 
	 * @returns boolean if the bind is successfull
	 */
	const bool bindCallback(const SDL_Keycode keycode, callbackFunction callback);

	/*
	 * @brief Delete a callback to a SDL keycode
	 *
	 * @param keycode - SDL keycode to unbind a command to
	 *
	 * @returns boolean if the unbind is successfull
	 */
	const bool unbindCallback(const SDL_Keycode keycode);

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
	 *
	 * @returns boolean if the callback runned successfull
	 */
	const bool runCallback(const SDL_Keycode keycode) const;

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
	 */
	bool addRunningCallback(const SDL_Keycode keycode);

	/*
	 * @brief remove a callback into m_runningCallbacks
	 *
	 * @param keycode - SDL keycode pressed
	 */
	void removeRunningCallback(const SDL_Keycode keycode);

private:
	std::map<SDL_Keycode, callbackFunction> m_callbacks;
	std::list<SDL_Keycode> m_runningCallbacks;
};

