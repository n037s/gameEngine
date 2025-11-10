#include "Sprite.h"

#include <iostream>

#include "ImageRenderer.h"
#include "AssetsManager.h"

Sprite::Sprite(point2D pos, std::string assetName)
{
	m_pos = pos;
	m_asset = static_cast<Image*>(AssetsManager::getInstance()->getAsset(assetName));
}

bool Sprite::createRenderer(SDL_Renderer* renderer)
{
	ImageRenderer* imageRenderer = new ImageRenderer(renderer, m_asset);
	m_renderer = imageRenderer;
	m_size = imageRenderer->getSize();

	return m_renderer->isGenerated();
}