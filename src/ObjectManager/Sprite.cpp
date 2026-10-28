#include "Sprite.h"

#include <iostream>

#include "ImageRenderer.h"
#include "AssetsManager.h"

Sprite::Sprite(point2D pos, const std::string& assetName) : Object(pos)
{
	auto asset = AssetsManager::getInstance()->getAsset(assetName);
	if (asset) 
		m_asset = static_cast<Image*>(asset);
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
	auto asset = AssetsManager::getInstance()->getAsset(members.getMember<std::string>("assetID"));
	if (asset)
		m_asset = static_cast<Image*>(asset);
}

ObjectMemberHolder Sprite::serialize() const
{
	ObjectMemberHolder objectMemberHolder = Object::serialize();
	std::string assetID = m_asset ? m_asset->getAssetID() : "";
	objectMemberHolder.addMember("assetID", assetID);
	return objectMemberHolder;
}

bool Sprite::setAsset(const std::string& assetID)
{
	auto manager = AssetsManager::getInstance();
	auto image = dynamic_cast<Image*>(manager->getAsset(assetID));
	if (!image || image->getAssetID() != assetID || !image->getSurface())
		return false;

	const auto center = getPos();
	m_asset = image;
	if (auto renderer = dynamic_cast<ImageRenderer*>(m_renderer))
		renderer->setAsset(image);
	m_size = size2D(image->getSurface()->w, image->getSurface()->h);
	setPos(center);
	return true;
}