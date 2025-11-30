#pragma once

#include <functional>
#include <unordered_map>
#include <string>

#include "Geometry.h"
#include "Object.h"

#include <iostream>

using buttonCallback = std::function<bool(point2D pos, Object* caller)>;

class ButtonCallbackRegistery
{
public:
	static ButtonCallbackRegistery* instance() {
		if (!m_instance)
		{
			m_instance = new ButtonCallbackRegistery();
		}
		return m_instance;
	}

	void registerCallback(const std::string& id, buttonCallback callback)
	{
		m_callbacks[id] = callback;
	}

	buttonCallback getCallback(const std::string& id) const
	{
		auto it = m_callbacks.find(id);
		return (it != m_callbacks.end()) ? it->second : nullptr;
	}

private:
	ButtonCallbackRegistery();

	static ButtonCallbackRegistery* m_instance;
	std::unordered_map<std::string, buttonCallback> m_callbacks;
};


