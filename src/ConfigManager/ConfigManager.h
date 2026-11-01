#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include "Logger.h"

namespace tinyxml2 { class XMLDocument; }

class ConfigManager
{
    public:
        virtual ~ConfigManager() = 0;

        ConfigManager(const ConfigManager&) = delete;
        ConfigManager& operator=(const ConfigManager&) = delete;

        void loadConfig();
        void saveConfig() const;

    protected:
        explicit ConfigManager(const std::filesystem::path& filePath);
        virtual void parseConfig();
        std::string getValue(const std::string& section, const std::string& parameter) const;
        void setValue(const std::string& section, const std::string& parameter, const std::string& value);

        std::filesystem::path getConfigFolder() const;

        Logger* m_logger;

    private:
        std::filesystem::path m_filePath;
        std::unique_ptr<tinyxml2::XMLDocument> m_document;
};