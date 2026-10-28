#include "SelectionBoundRenderer.h"


#include <algorithm>
#include <cmath>

bool SelectionBoundsRenderer::renderQuad(const std::array<point2D, 4>& corners)
    {
        bool success = true;
        constexpr float cornerLength = 25.0f;
        constexpr float edgeWidth = 4.0f;
        for (size_t index = 0; index < corners.size(); ++index)
        {
            const point2D& corner = corners[index];
            const std::array<point2D, 2> neighbors = {
                corners[(index + 1) % corners.size()],
                corners[(index + corners.size() - 1) % corners.size()]
            };

            for (const point2D& neighbor : neighbors)
            {
                const float dx = neighbor.x - corner.x;
                const float dy = neighbor.y - corner.y;
                const float edgeLength = std::hypot(dx, dy);
                if (edgeLength <= 0.0f)
                    continue;

                const float length = std::min(cornerLength, edgeLength * 0.35f);
                const point2D end(corner.x + length * dx / edgeLength,
                    corner.y + length * dy / edgeLength);
                success = renderThickLine(corner, end, edgeWidth) && success;
            }
        }
        return success;
    }

    bool SelectionBoundsRenderer::renderThickLine(const point2D& start, const point2D& end, float width)
    {
        const float dx = end.x - start.x;
        const float dy = end.y - start.y;
        const float length = std::hypot(dx, dy);
        if (length <= 0.0f)
            return true;

        const float halfWidth = width / 2.0f;
        const point2D normal(-dy * halfWidth / length, dx * halfWidth / length);
        const SDL_FColor color{ 1.0f, 1.0f, 1.0f, 1.0f };
        const SDL_Vertex vertices[] = {
            { (start + normal).toSDL(), color, { 0.0f, 0.0f } },
            { (end + normal).toSDL(), color, { 0.0f, 0.0f } },
            { (end - normal).toSDL(), color, { 0.0f, 0.0f } },
            { (start - normal).toSDL(), color, { 0.0f, 0.0f } }
        };
        const int indices[] = { 0, 1, 2, 0, 2, 3 };
        return SDL_RenderGeometry(m_renderer, nullptr, vertices, 4, indices, 6);
    }