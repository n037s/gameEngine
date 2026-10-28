#pragma once

#include <string>

#include "Object.h"

#include "Image.h"

class Sprite : public Object
{
public:
	Sprite(const ObjectMemberHolder& members);
	Sprite(point2D pos, const std::string& assetName);

	std::string getTypeName() const override { return "Sprite"; }
	ObjectMemberHolder serialize() const override;

	bool createRenderer(SDL_Renderer* renderer) override;
	const point2D getPos() const override { return m_pos + m_size / 2; }
	const rect2D getShape() const override { return rect2D(m_pos, m_size); };

private:
	Image* m_asset{ nullptr };
};

