#pragma once

#include <chrono>
#include <cstddef>

class FrameRate
{
public:
    static FrameRate& getInstance()
    {
        static FrameRate instance;
        return instance;
    }

    FrameRate(const FrameRate&) = delete;
    FrameRate& operator=(const FrameRate&) = delete;

    void reset(std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now());
    void update(std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now());
    const float getDistanceFromSpeed(float speed) const;

    int getFPS() const { return m_fps; }
    float getDeltaTimeSeconds() const { return m_deltaTimeSeconds; }

private:
    FrameRate() = default;

    std::chrono::steady_clock::time_point m_lastTime{ std::chrono::steady_clock::now() };
    std::chrono::steady_clock::time_point m_lastFrameTime{ m_lastTime };
    std::size_t m_frameCount{ 0 };
    int m_fps{ 0 };
    float m_deltaTimeSeconds{ 0.0f };
};