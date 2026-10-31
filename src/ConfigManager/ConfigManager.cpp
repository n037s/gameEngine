#include "ConfigManager.h"

#include <stdexcept>
#include <tinyxml2.h>
#include <iostream>

namespace
{
	tinyxml2::XMLElement& findParameter(tinyxml2::XMLDocument& document,
		const std::string& section, const std::string& parameter)
	{
		auto* sectionNode = document.RootElement()->FirstChildElement(section.c_str());
		auto* parameterNode = sectionNode ? sectionNode->FirstChildElement(parameter.c_str()) : nullptr;
		if (!parameterNode)
			throw std::out_of_range("Missing configuration parameter: " + section + "/" + parameter);
		return *parameterNode;
	}
}

ConfigManager::ConfigManager(const std::filesystem::path& filePath) : m_filePath(CONFIG_FOLDER / filePath)
{
}

ConfigManager::~ConfigManager() = default;

void ConfigManager::loadConfig()
{
	auto document = std::make_unique<tinyxml2::XMLDocument>();
	if (document->LoadFile(m_filePath.string().c_str()) != tinyxml2::XML_SUCCESS)
		throw std::runtime_error("Cannot load configuration '" + m_filePath.string() + "': " + document->ErrorStr());

	const auto* root = document->RootElement();
	if (!root || document->FirstChildElement() != document->LastChildElement()
		|| m_filePath.stem().string() != root->Name())
		throw std::runtime_error("Configuration root must match file name: " + m_filePath.stem().string());

	m_document.swap(document);
	try
	{
		parseConfig();
	}
	catch (...)
	{
		m_document.swap(document);
		throw;
	}
}

void ConfigManager::parseConfig()
{
    std::cout << "[ERROR] Parsing of configuration file not defined" << std::endl;
}

std::filesystem::path ConfigManager::getConfigFolder() const
{
	return CONFIG_FOLDER;
}

void ConfigManager::saveConfig() const
{
	if (m_document->SaveFile(m_filePath.string().c_str()) != tinyxml2::XML_SUCCESS)
		throw std::runtime_error("Cannot save configuration: " + m_filePath.string());
}

std::string ConfigManager::getValue(const std::string& section, const std::string& parameter) const
{
	const auto& node = findParameter(*m_document, section, parameter);
	if (node.FirstChildElement())
		throw std::runtime_error("Configuration parameter must contain text: " + section + "/" + parameter);
	const char* text = node.GetText();
	return text ? text : "";
}

void ConfigManager::setValue(const std::string& section, const std::string& parameter, const std::string& value)
{
	auto& node = findParameter(*m_document, section, parameter);
	if (node.FirstChildElement())
		throw std::runtime_error("Configuration parameter must contain text: " + section + "/" + parameter);
	node.SetText(value.c_str());
}
