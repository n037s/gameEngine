#pragma once
#include "Sprite.h"

#include <vector>
#include <chrono>

class AnimatedSprite :
    public Object
{
public:
	AnimatedSprite(point2D pos, std::vector<std::string> assetNames, int animationSpeedMS);

	bool createRenderer(SDL_Renderer* renderer) override;
	const point2D getPos() const override { return m_pos + m_size / 2; }
	const rect2D getShape() const override { return rect2D(m_pos - m_size / 2, m_size); };

	void update() override;

private:
	int rotation_count{ 0 };
	Image* m_currentAsset{ nullptr };
	std::vector<Image*> m_assets{ nullptr };

	std::chrono::steady_clock::time_point m_lastTime;
	int m_animationSpeed = 500;
};

