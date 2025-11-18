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
		std::cout << "register a callback : " << id << std::endl;
		m_callbacks[id] = callback;
	}

	buttonCallback getCallback(const std::string& id) const
	{
		auto it = m_callbacks.find(id);
		std::cout << m_callbacks.size() << std::endl;
		for (auto i : m_callbacks)
		{
			std::cout << i.first << std::endl;
		}
		std::cout << "retrieved callback " << id << " " << ((it != m_callbacks.end()) ? "true" : "false") << std::endl;
		return (it != m_callbacks.end()) ? it->second : nullptr;
	}

private:
	ButtonCallbackRegistery() { m_callbacks = std::unordered_map<std::string, buttonCallback>(); }

	static ButtonCallbackRegistery* m_instance;
	std::unordered_map<std::string, buttonCallback> m_callbacks;
};


