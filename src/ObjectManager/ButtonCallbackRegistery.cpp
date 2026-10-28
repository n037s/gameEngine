#include "ButtonCallbackRegistery.h"

ButtonCallbackRegistery* ButtonCallbackRegistery::m_instance = nullptr;

bool Nothing(point2D pos, Object* caller)
{
	return true;
}

ButtonCallbackRegistery::ButtonCallbackRegistery() 
{ 
	m_callbacks = std::unordered_map<std::string, buttonCallback>(); 
	registerCallback("Nothing", Nothing);
}