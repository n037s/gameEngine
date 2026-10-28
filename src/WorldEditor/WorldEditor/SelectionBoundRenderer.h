#pragma once

#include "BaseRenderer.h"

class SelectionBoundsRenderer final : public BaseRenderer
{
public:
    explicit SelectionBoundsRenderer(SDL_Renderer* renderer) : BaseRenderer(renderer) {}

    bool renderQuad(const std::array<point2D, 4>& corners) override;

private:
    bool renderThickLine(const point2D& start, const point2D& end, float width);
};