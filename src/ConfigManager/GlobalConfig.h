#pragma once

#include "ConfigManager.h"

class GlobalConfig : public ConfigManager
{
    public: 
        static GlobalConfig& getInstance();

        int getFrameRateLimit() const;
        void setFrameRateLimit(int limit);

    private: 
        GlobalConfig();
        ~GlobalConfig() override = default;
        void parseConfig() override;

        int m_frameRateLimit{0};
};