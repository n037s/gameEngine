#pragma once

#include "Geometry.h"
#include <array>

class Camera
{
public:
    enum class ViewMode { TopDown, Isometric };

    static Camera* getInstance() {
        if (!m_instance)
            m_instance = new Camera(point2D(0,0), size2D(0,0));
        return m_instance;
    }

    point2D getPos() const { return m_pos; }
    size2D getWindowSize() const { return windowSize; }
    void setPos(const point2D& pos) { m_pos = pos; }
    void setWindowSize(const size2D& size) { windowSize = size; }
    void setCameraZLimits(float zMin, float zMax);
    void setViewMode(ViewMode mode) { m_viewMode = mode; }
    ViewMode getViewMode() const { return m_viewMode; }
    void toggleViewMode();
    void setIsometricAngles(float tiltDegrees, float headingDegrees);

    void movePos(const point2D& dpos);
    void moveZ(float z);
    float getZ() const { return m_height; }
    float getRenderingScale() const;

    rect2D getRenderingRect() const;
    point2D worldToWindowPoint(const point2D& position) const;
    point2D windowToWorldPoint(const point2D& position) const;
    point2D windowToWorldDelta(const point2D& delta) const;
    std::array<point2D, 4> getRenderingCorners(const rect2D& shape, bool steady = false) const;
    bool intersectsWindowRect(const rect2D& worldShape, const rect2D& windowRect, bool steady = false) const;
    rect2D WorldToWindow(const rect2D& objShape) const;
    rect2D WindowToWorld(const rect2D& windowCoords) const;

private:
    static Camera* m_instance;
    Camera(point2D pos, size2D windowSize) : m_pos{ pos }, windowSize(windowSize) {};

    point2D m_pos;
    float m_height{ 1 };
    float m_minCameraZ{ 0.05f };
    float m_maxCameraZ{ 5 };
    ViewMode m_viewMode{ ViewMode::TopDown };
    float m_tiltDegrees{ 50.0f };
    float m_headingDegrees{ 20.0f };
    size2D windowSize;
};
