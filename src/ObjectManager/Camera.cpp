#include "Camera.h"

#include <iostream>
#include <algorithm>
#include <cmath>

namespace
{
    constexpr float degreesToRadians = 0.017453292519943295f;

    rect2D bounds(const std::array<point2D, 4>& corners)
    {
        float minX = corners[0].x;
        float maxX = minX;
        float minY = corners[0].y;
        float maxY = minY;
        for (const point2D& corner : corners)
        {
            minX = std::min(minX, corner.x);
            maxX = std::max(maxX, corner.x);
            minY = std::min(minY, corner.y);
            maxY = std::max(maxY, corner.y);
        }
        return rect2D(minX, minY, maxX - minX, maxY - minY);
    }
}

Camera* Camera::m_instance = nullptr;

rect2D Camera::getRenderingRect() const
{
    return WindowToWorld(rect2D(point2D(0, 0), windowSize));
}

rect2D Camera::WorldToWindow(const rect2D& objShape) const
{
    return bounds(getRenderingCorners(objShape));
}

rect2D Camera::WindowToWorld(const rect2D& windowCoords) const
{
    const point2D topLeft = windowCoords.position;
    return bounds({ windowToWorldPoint(topLeft),
        windowToWorldPoint(topLeft + point2D(windowCoords.size.w, 0)),
        windowToWorldPoint(topLeft + point2D(windowCoords.size.w, windowCoords.size.h)),
        windowToWorldPoint(topLeft + point2D(0, windowCoords.size.h)) });
}

point2D Camera::worldToWindowPoint(const point2D& position) const
{
    point2D projected = position - m_pos;
    if (m_viewMode == ViewMode::Isometric)
    {
        const float heading = m_headingDegrees * degreesToRadians;
        const float tilt = m_tiltDegrees * degreesToRadians;
        projected = point2D(std::cos(heading) * projected.x - std::sin(heading) * projected.y,
            std::cos(tilt) * (std::sin(heading) * projected.x + std::cos(heading) * projected.y));
    }
    return projected / m_height + windowSize / 2;
}

point2D Camera::windowToWorldDelta(const point2D& delta) const
{
    point2D unprojected = delta * m_height;
    if (m_viewMode == ViewMode::Isometric)
    {
        const float heading = m_headingDegrees * degreesToRadians;
        const float tilt = m_tiltDegrees * degreesToRadians;
        unprojected.y /= std::cos(tilt);
        unprojected = point2D(std::cos(heading) * unprojected.x + std::sin(heading) * unprojected.y,
            -std::sin(heading) * unprojected.x + std::cos(heading) * unprojected.y);
    }
    return unprojected;
}

point2D Camera::windowToWorldPoint(const point2D& position) const
{
    return m_pos + windowToWorldDelta(position - windowSize / 2);
}

std::array<point2D, 4> Camera::getRenderingCorners(const rect2D& shape, bool steady) const
{
    if (steady && m_viewMode == ViewMode::Isometric)
    {
        const point2D anchor = worldToWindowPoint(shape.position + point2D(shape.size.w / 2, shape.size.h));
        const size2D size = shape.size * getRenderingScale();
        const point2D topLeft = anchor - point2D(size.w / 2, size.h);
        return { topLeft, topLeft + point2D(size.w, 0),
            topLeft + point2D(size.w, size.h), topLeft + point2D(0, size.h) };
    }
    return { worldToWindowPoint(shape.position),
        worldToWindowPoint(shape.position + point2D(shape.size.w, 0)),
        worldToWindowPoint(shape.position + point2D(shape.size.w, shape.size.h)),
        worldToWindowPoint(shape.position + point2D(0, shape.size.h)) };
}

void Camera::toggleViewMode()
{
    m_viewMode = m_viewMode == ViewMode::TopDown ? ViewMode::Isometric : ViewMode::TopDown;
}

void Camera::setIsometricAngles(float tiltDegrees, float headingDegrees)
{
    if (std::isfinite(tiltDegrees) && std::isfinite(headingDegrees))
    {
        m_tiltDegrees = std::clamp(tiltDegrees, -89.0f, 89.0f);
        m_headingDegrees = std::remainder(headingDegrees, 360.0f);
    }
}

void Camera::setCameraZLimits(float zMin, float zMax)
{
    if (!std::isfinite(zMin) || !std::isfinite(zMax))
        return;
    m_minCameraZ = std::max(0.05f, zMin);
    m_maxCameraZ = std::max(m_minCameraZ, zMax);
    m_height = std::clamp(m_height, m_minCameraZ, m_maxCameraZ);
}

void Camera::movePos(const point2D& dpos)
{
    m_pos = m_pos + dpos;
}

void Camera::moveZ(float dz)
{
    m_height = std::clamp(m_height + dz, m_minCameraZ, m_maxCameraZ);
}

float Camera::getRenderingScale() const
{
    return 1 / m_height;
}

bool Camera::intersectsWindowRect(const rect2D& worldShape, const rect2D& windowRect, bool steady) const
{
    const auto corners = getRenderingCorners(worldShape, steady);
    const std::array<point2D, 4> mouseCorners = { windowRect.position,
        windowRect.position + point2D(windowRect.size.w, 0),
        windowRect.position + point2D(windowRect.size.w, windowRect.size.h),
        windowRect.position + point2D(0, windowRect.size.h) };
    const point2D firstEdge = corners[1] - corners[0];
    const point2D secondEdge = corners[3] - corners[0];
    const std::array<point2D, 4> axes = { point2D(1, 0), point2D(0, 1),
        point2D(-firstEdge.y, firstEdge.x), point2D(-secondEdge.y, secondEdge.x) };
    for (const point2D& axis : axes)
    {
        float minWorld = corners[0].x * axis.x + corners[0].y * axis.y;
        float maxWorld = minWorld;
        float minMouse = mouseCorners[0].x * axis.x + mouseCorners[0].y * axis.y;
        float maxMouse = minMouse;
        for (size_t index = 1; index < corners.size(); ++index)
        {
            const float worldProjection = corners[index].x * axis.x + corners[index].y * axis.y;
            const float mouseProjection = mouseCorners[index].x * axis.x + mouseCorners[index].y * axis.y;
            minWorld = std::min(minWorld, worldProjection);
            maxWorld = std::max(maxWorld, worldProjection);
            minMouse = std::min(minMouse, mouseProjection);
            maxMouse = std::max(maxMouse, mouseProjection);
        }
        if (maxWorld <= minMouse || maxMouse <= minWorld)
            return false;
    }
    return true;
}