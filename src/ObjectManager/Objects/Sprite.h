#pragma once

#include <string>

#include "Object.h"

#include "Image.h"

class Sprite : public Object
{
public:
	Sprite(const ObjectMemberHolder& members);
	Sprite(point2D pos, const std::string& assetName = "default");

	std::string getTypeName() const override { return "Sprite"; }
	ObjectMemberHolder serialize() const override;

	bool createRenderer(SDL_Renderer* renderer) override;
	bool setAsset(const std::string& assetID);
	const point2D getPos() const override { return m_pos + m_size / 2; }
	void setPos(const point2D& pos) override { m_pos = pos - m_size / 2; }
	const rect2D getShape() const override { return rect2D(m_pos, m_size); };

private:
	Image* m_asset{ nullptr };
};

