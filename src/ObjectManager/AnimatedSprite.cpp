#include "AnimatedSprite.h"

#include "ImageRenderer.h"
#include "AssetsManager.h"

#include <SDL3_image/SDL_image.h>
#include <algorithm>
#include <iostream>

AnimatedSprite::AnimatedSprite(point2D pos, bool steady, const std::vector<std::string>& assetNames, int animationSpeedMS) : Object(pos)
{
	m_steady = steady;
	m_assets = std::vector<Image*>();
	m_animationSpeed = animationSpeedMS;

	for (const auto& assetName : assetNames)
	{
		auto asset = AssetsManager::getInstance()->getAsset(assetName);
		if (asset)
			m_assets.push_back(static_cast<Image*>(asset));
	}
	if (!m_assets.empty())
		m_currentAsset = m_assets.front();
}

AnimatedSprite::AnimatedSprite(point2D pos, bool steady, int animationSpeedMS) : Object(pos)
{
	m_steady = steady;
	m_assets = std::vector<Image*>();
	m_animationSpeed = animationSpeedMS;

	auto asset = AssetsManager::getInstance()->getAsset("default");
	if (asset)
		m_assets.push_back(static_cast<Image*>(asset));
	if (!m_assets.empty())
		m_currentAsset = m_assets.at(0);
}

bool AnimatedSprite::createRenderer(SDL_Renderer* renderer)
{
	delete m_renderer;
	ImageRenderer* imageRenderer = new ImageRenderer(renderer, m_currentAsset);
	m_renderer = imageRenderer;
	m_size = imageRenderer->getSize();

	return m_renderer->isGenerated();
}

bool AnimatedSprite::setAnimation(const std::string& assetID)
{
	auto manager = AssetsManager::getInstance();
	const auto animationIDs = manager->getAnimatedAssets();
	if (std::find(animationIDs.begin(), animationIDs.end(), assetID) == animationIDs.end())
		return false;

	auto asset = dynamic_cast<Image*>(manager->getAsset(assetID));
	if (!asset || !asset->getSurface())
		return false;

	std::unique_ptr<IMG_Animation, decltype(&IMG_FreeAnimation)> animation(
		IMG_LoadAnimation(manager->getPath(assetID).c_str()), IMG_FreeAnimation);
	if (!animation || animation->count <= 0)
		return false;

	std::vector<std::unique_ptr<Image>> ownedAssets;
	std::vector<Image*> assets;
	ownedAssets.reserve(animation->count);
	assets.reserve(animation->count);
	for (int frameIndex = 0; frameIndex < animation->count; ++frameIndex)
	{
		if (!animation->frames[frameIndex])
			return false;
		auto frame = std::make_unique<Image>(animation->frames[frameIndex]);
		animation->frames[frameIndex] = nullptr;
		frame->setAssetID(assetID);
		frame->setpath(manager->getPath(assetID));
		assets.push_back(frame.get());
		ownedAssets.push_back(std::move(frame));
	}

	if (m_renderer)
		static_cast<ImageRenderer*>(m_renderer)->resetAsset(assets.front());
	m_assets = std::move(assets);
	m_ownedAssets = std::move(ownedAssets);
	m_animationID = assetID;
	m_currentAsset = m_assets.front();
	rotation_count = 0;
	m_lastTime = std::chrono::steady_clock::now();
	if (m_renderer)
		m_size = m_renderer->getSize();
	return true;
}

AnimatedSprite::AnimatedSprite(const ObjectMemberHolder& members) : Object(members)
{
	m_animationSpeed = members.getMember<int>("AnimationSpeed");
	const auto animationID = members.getMember<std::string>("AnimationID");
	if (!animationID.empty() && setAnimation(animationID))
		return;
	m_assets = std::vector<Image*>();
	for (int i = 0; i < members.getMember<size_t>("AssetsCount"); ++i)
	{
		std::string assetName = members.getMember<std::string>("AssetID" + std::to_string(i+1));
		auto asset = AssetsManager::getInstance()->getAsset(assetName);
		if (asset)
			m_assets.push_back(static_cast<Image*>(asset));
	}
	if (!m_assets.empty())
		m_currentAsset = m_assets.front();
}

ObjectMemberHolder AnimatedSprite::serialize() const
{
	ObjectMemberHolder objectMemberHolder = Object::serialize();
	objectMemberHolder.addMember("AssetsCount", m_animationID.empty() ? m_assets.size() : size_t{ 0 });
	if (!m_animationID.empty())
		objectMemberHolder.addMember("AnimationID", m_animationID);
	int i = 0;
	for (auto asset : m_assets)
	{
		if (!m_animationID.empty())
			break;
		std::string assetID = asset ? asset->getAssetID() : "";
		objectMemberHolder.addMember("AssetID" + std::to_string(++i), assetID);
	}
	objectMemberHolder.addMember("AnimationSpeed", m_animationSpeed);
	return objectMemberHolder;
}

void AnimatedSprite::update()
{
	if (m_assets.empty() || !m_renderer)
		return;
	// need to update with a frame rate. Not all the time. 
	auto currentTime = std::chrono::steady_clock::now();
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