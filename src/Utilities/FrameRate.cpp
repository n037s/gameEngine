#include "FrameRate.h"

void FrameRate::reset(std::chrono::steady_clock::time_point currentTime)
{
    m_lastTime = currentTime;
    m_lastFrameTime = currentTime;
    m_frameCount = 0;
    m_fps = 0;
    m_deltaTimeSeconds = 0.0f;
}

void FrameRate::update(std::chrono::steady_clock::time_point currentTime)
{
    m_deltaTimeSeconds = std::chrono::duration<float>(currentTime - m_lastFrameTime).count();
    m_lastFrameTime = currentTime;
    ++m_frameCount;
    const auto elapsed = currentTime - m_lastTime;
    if (elapsed >= std::chrono::milliseconds(500))
    {
        m_fps = static_cast<int>(m_frameCount / std::chrono::duration<double>(elapsed).count());
        m_frameCount = 0;
        m_lastTime = currentTime;
    }
}

const float FrameRate::getDistanceFromSpeed(float speed) const
{
    return speed * m_deltaTimeSeconds;
}