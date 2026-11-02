#pragma once

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <string>

namespace Components
{
    enum Components
    {
        Logger = 0, 
        ConfigManager, 
        ControlManager,
        ObjectManager, 
        PlayerManager, 
        Utilities, 
        Viewer, 
        WorldEditor, 
        Main
    };
}

class Logger
{
    public:
        static Logger* getInstance(const std::string& component);
        static Logger* getInstance(Components::Components component);
        ~Logger() = default;

        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;

        void log(const char* message);
        void log(const std::string& message);

        void error(const char* message);
        void error(const std::string& message);

        void warning(const char* message);
        void warning(const std::string& message);

        void info(const char* message);
        void info(const std::string& message);


    private:
        explicit Logger(const std::string& component);
        void write(const char* level, const std::string& message);
        void open();
        void rotate();

        static constexpr std::uintmax_t MAX_FILE_SIZE = 5 * 1024 * 1024;
        static constexpr unsigned int BACKUP_COUNT = 3;

        std::filesystem::path m_filePath;
        std::ofstream m_file;
        std::uintmax_t m_fileSize = 0;
        std::mutex m_mutex;
};