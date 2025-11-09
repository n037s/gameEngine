#pragma once

#include <string>

#include "Object.h"

class Sprite : public Object
{
public:
	Sprite(point2D pos, std::string assetName);

	bool createRenderer(SDL_Renderer* renderer) override;
	const point2D getPos() const override { return m_pos + m_size / 2; }
	const rect2D getShape() const override { return rect2D(m_pos - m_size / 2, m_size); };

private:
	std::string m_assetPath{ "" };
};

