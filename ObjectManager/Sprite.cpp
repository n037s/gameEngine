#include "Sprite.h"

#include <iostream>

#include "ImageRenderer.h"
#include "AssetsManager.h"

Sprite::Sprite(point2D pos, const std::string& assetName) : Object(pos)
{
	m_asset = static_cast<Image*>(AssetsManager::getInstance()->getAsset(assetName));
}

bool Sprite::createRenderer(SDL_Renderer* renderer)
{
	ImageRenderer* imageRenderer = new ImageRenderer(renderer, m_asset);
	m_renderer = imageRenderer;
	m_size = imageRenderer->getSize();

	return m_renderer->isGenerated();
}

Sprite::Sprite(const ObjectMemberHolder& members) : Object(members)
{
	m_asset = static_cast<Image*>(AssetsManager::getInstance()->getAsset(members.getMember<std::string>("assetID")));
}

ObjectMemberHolder Sprite::serialize() const
{
	ObjectMemberHolder objectMemberHolder = Object::serialize();
	std::string assetID = m_asset ? m_asset->getAssetID() : "";
	objectMemberHolder.addMember("assetID", assetID);
	return objectMemberHolder;
}