#include "ConfigManager.h"

#include <stdexcept>
#include <tinyxml2.h>

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

ConfigManager::ConfigManager(const std::filesystem::path& filePath)
	: m_logger(Logger::getInstance(Components::ConfigManager)), m_filePath(CONFIG_FOLDER / filePath)
{
}

ConfigManager::~ConfigManager() = default;

void ConfigManager::loadConfig()
{
	auto document = std::make_unique<tinyxml2::XMLDocument>();
	if (document->LoadFile(m_filePath.string().c_str()) != tinyxml2::XML_SUCCESS)
	{
		const auto message = "Cannot load configuration '" + m_filePath.string() + "': " + document->ErrorStr();
		m_logger->error(message);
		throw std::runtime_error(message);
	}

	const auto* root = document->RootElement();
	if (!root || document->FirstChildElement() != document->LastChildElement()
		|| m_filePath.stem().string() != root->Name())
	{
		const auto message = "Configuration root must match file name: " + m_filePath.stem().string();
		m_logger->error(message);
		throw std::runtime_error(message);
	}

	m_document.swap(document);
	try
	{
		parseConfig();
	}
	catch (const std::exception& exception)
	{
		m_document.swap(document);
		m_logger->error("Cannot parse configuration '" + m_filePath.string() + "': " + exception.what());
		throw;
	}
	catch (...)
	{
		m_document.swap(document);
		m_logger->error("Cannot parse configuration: " + m_filePath.string());
		throw;
	}
	m_logger->info("Loaded configuration: " + m_filePath.string());
}

void ConfigManager::parseConfig()
{
	m_logger->error("Parsing of configuration file not defined: " + m_filePath.string());
}

std::filesystem::path ConfigManager::getConfigFolder() const
{
	return CONFIG_FOLDER;
}

void ConfigManager::saveConfig() const
{
	if (m_document->SaveFile(m_filePath.string().c_str()) != tinyxml2::XML_SUCCESS)
	{
		const auto message = "Cannot save configuration: " + m_filePath.string();
		m_logger->error(message);
		throw std::runtime_error(message);
	}
	m_logger->info("Saved configuration: " + m_filePath.string());
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
