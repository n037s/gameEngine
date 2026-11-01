#include "Logger.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <iostream>

Logger* Logger::getInstance(const std::string& component)
{
	static std::mutex registryMutex;
	static std::map<std::string, std::unique_ptr<Logger>> instances;
	std::lock_guard<std::mutex> lock(registryMutex);
	auto found = instances.find(component);
	if (found != instances.end())
		return found->second.get();

	auto logger = std::unique_ptr<Logger>(new Logger(component));
	auto* instance = logger.get();
	instances.emplace(component, std::move(logger));
	return instance;
}

Logger* Logger::getInstance(Components::Components component)
{
	static const std::map<Components::Components, std::string> componentNames = {
		{ Components::Logger, "Logger" },
		{ Components::ConfigManager, "ConfigManager" },
		{ Components::ControlManager, "ControlManager" },
		{ Components::ObjectManager, "ObjectManager" },
		{ Components::PlayerManager, "PlayerManager" },
		{ Components::Utilities, "Utilities" },
		{ Components::Viewer, "Viewer" },
		{ Components::WorldEditor, "WorldEditor" },
		{ Components::Main, "Main" }
	};
    auto it = componentNames.find(component);
    if (it == componentNames.end())
        throw std::invalid_argument("Unknown component");
    return getInstance(it->second);
}

Logger::Logger(const std::string& component)
{
	if (component.empty() || component.find_first_not_of(
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_-") != std::string::npos)
		throw std::invalid_argument("Logger component names must contain only letters, digits, '_' or '-'");

	m_filePath = std::filesystem::absolute(std::filesystem::path("logs") / (component + ".log"));
	std::filesystem::create_directories(m_filePath.parent_path());
	m_file.exceptions(std::ios::failbit | std::ios::badbit);
	open();
}

void Logger::open()
{
	m_file.clear();
	m_file.open(m_filePath, std::ios::out | std::ios::app | std::ios::binary);
	m_fileSize = std::filesystem::file_size(m_filePath);
}

void Logger::rotate()
{
	m_file.close();
	std::filesystem::remove(m_filePath.string() + "." + std::to_string(BACKUP_COUNT));
	for (unsigned int backup = BACKUP_COUNT; backup > 1; --backup)
	{
		const auto source = m_filePath.string() + "." + std::to_string(backup - 1);
		if (std::filesystem::exists(source))
			std::filesystem::rename(source, m_filePath.string() + "." + std::to_string(backup));
	}
	std::filesystem::rename(m_filePath, m_filePath.string() + ".1");
	open();
}

void Logger::write(const char* level, const std::string& message)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	const auto now = std::chrono::system_clock::now();
	const auto time = std::chrono::system_clock::to_time_t(now);
	std::tm localTime{};
	if (localtime_s(&localTime, &time) != 0)
		throw std::runtime_error("Cannot obtain log timestamp");

	std::ostringstream entry;
	entry << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S") << " [" << level << "] " << message << '\n';
	const auto record = entry.str();
	if (!m_file.is_open())
		open();
	if (m_fileSize > 0 && (m_fileSize >= MAX_FILE_SIZE || record.size() > MAX_FILE_SIZE - m_fileSize))
		rotate();
	m_file << record;
	m_file.flush();
	m_fileSize += record.size();

	// Also log in the console
	std::cout << record;
}

void Logger::log(const char* message) { log(std::string(message ? message : "")); }
void Logger::log(const std::string& message) { write("LOG", message); }
void Logger::error(const char* message) { error(std::string(message ? message : "")); }
void Logger::error(const std::string& message) { write("ERROR", message); }
void Logger::warning(const char* message) { warning(std::string(message ? message : "")); }
void Logger::warning(const std::string& message) { write("WARNING", message); }
void Logger::info(const char* message) { info(std::string(message ? message : "")); }
void Logger::info(const std::string& message) { write("INFO", message); }
