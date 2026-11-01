#include "GlobalConfig.h"

#include "Logger.h"

#include <charconv>
#include <stdexcept>

GlobalConfig::GlobalConfig() : ConfigManager("globalConfig.xml")
{
	loadConfig();
}

GlobalConfig& GlobalConfig::getInstance()
{
	static GlobalConfig instance;
	return instance;
}

int GlobalConfig::getFrameRateLimit() const
{
	return m_frameRateLimit;
}

void GlobalConfig::parseConfig()
{
	const std::string value = getValue("viewer", "frameRateLimit");
	const auto first = value.find_first_not_of(" \t\r\n");
	const auto last = value.find_last_not_of(" \t\r\n");
	if (first == std::string::npos)
	{
		m_logger->error("viewer/frameRateLimit must be a non-negative integer");
		throw std::runtime_error("viewer/frameRateLimit must be a non-negative integer");
	}

	int limit = 0;
	const char* end = value.data() + last + 1;
	const auto result = std::from_chars(value.data() + first, end, limit);
	if (result.ec != std::errc() || result.ptr != end || limit < 0)
	{
		m_logger->error("viewer/frameRateLimit must be a non-negative integer");
		throw std::runtime_error("viewer/frameRateLimit must be a non-negative integer");
	}
	m_frameRateLimit = limit;
}

void GlobalConfig::setFrameRateLimit(int limit)
{
	if (limit < 0)
	{
		m_logger->error("Frame rate limit cannot be negative");
		throw std::invalid_argument("Frame rate limit cannot be negative");
	}
	setValue("viewer", "frameRateLimit", std::to_string(limit));
	m_frameRateLimit = limit;
}
