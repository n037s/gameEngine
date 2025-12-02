#include "AnimatedSprite.h"

#include "ImageRenderer.h"
#include "AssetsManager.h"

#include <iostream>

AnimatedSprite::AnimatedSprite(point2D pos, const std::vector<std::string>& assetNames, int animationSpeedMS) : Object(pos)
{
	m_assets = std::vector<Image*>();
	m_animationSpeed = animationSpeedMS;

	for (const auto& assetName : assetNames)
	{
		auto asset = AssetsManager::getInstance()->getAsset(assetName);
		if (asset)
			m_assets.push_back(static_cast<Image*>(asset));
	}
	m_currentAsset = m_assets.at(0);
}

bool AnimatedSprite::createRenderer(SDL_Renderer* renderer)
{
	ImageRenderer* imageRenderer = new ImageRenderer(renderer, m_currentAsset);
	m_renderer = imageRenderer;
	m_size = imageRenderer->getSize();

	return m_renderer->isGenerated();
}

AnimatedSprite::AnimatedSprite(const ObjectMemberHolder& members) : Object(members)
{
	m_assets = std::vector<Image*>();
	for (int i = 0; i < members.getMember<size_t>("AssetsCount"); ++i)
	{
		std::string assetName = members.getMember<std::string>("AssetID" + std::to_string(i+1));
		auto asset = AssetsManager::getInstance()->getAsset(assetName);
		if (asset)
			m_assets.push_back(static_cast<Image*>(asset));
	}
	m_currentAsset = m_assets.at(0);
	m_animationSpeed = members.getMember<int>("AnimationSpeed");
}

ObjectMemberHolder AnimatedSprite::serialize() const
{
	ObjectMemberHolder objectMemberHolder = Object::serialize();
	objectMemberHolder.addMember("AssetsCount", m_assets.size());
	int i = 0;
	for (auto asset : m_assets)
	{
		std::string assetID = asset ? asset->getAssetID() : "";
		objectMemberHolder.addMember("AssetID" + std::to_string(++i), assetID);
	}
	objectMemberHolder.addMember("AnimationSpeed", m_animationSpeed);
	return objectMemberHolder;
}

void AnimatedSprite::update()
{
	// need to update with a frame rate. Not all the time. 
	auto currentTime = std::chrono::high_resolution_clock::now();
	auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - m_lastTime);

	if (duration_ms.count() > m_animationSpeed)
	{
		m_lastTime = currentTime;

		rotation_count = (rotation_count < m_assets.size()-1 ) ? rotation_count + 1 : 0;

		if (!m_assets.empty())
		{
			m_currentAsset = m_assets.at(rotation_count);
			static_cast<ImageRenderer*>(m_renderer)->setAsset(m_currentAsset);
		}
	}
}