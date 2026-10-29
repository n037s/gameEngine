#pragma once
#include "Sprite.h"

#include <vector>
#include <chrono>
#include <memory>

class AnimatedSprite :
    public Object
{
public:
	AnimatedSprite(const ObjectMemberHolder& members);
	AnimatedSprite(point2D pos, bool steady, const std::vector<std::string>& assetNames, int animationSpeedMS);
	AnimatedSprite(point2D pos, bool steady, int animationSpeedMS);

	bool createRenderer(SDL_Renderer* renderer) override;
	bool setAnimation(const std::string& assetID);

	std::string getTypeName() const override { return "AnimatedSprite"; }
	ObjectMemberHolder serialize() const override;

	const point2D getPos() const override { return m_pos + m_size / 2; }
	const point2D getAnchor() const { return m_pos; }
	void setPos(const point2D& pos) override { m_pos = pos - m_size / 2; }
	const rect2D getShape() const override { return rect2D(m_pos, m_size); };

	void update() override;
	void updateAnimation();

private:
	int rotation_count{ 0 };
	Image* m_currentAsset{ nullptr };
	std::vector<Image*> m_assets;
	std::vector<std::unique_ptr<Image>> m_ownedAssets;
	std::string m_animationID;

	std::chrono::steady_clock::time_point m_lastTime{ std::chrono::steady_clock::now() };
	int m_animationSpeed = 500;
};

