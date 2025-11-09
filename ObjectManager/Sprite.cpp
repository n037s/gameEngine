#include "Sprite.h"

#include <iostream>

#include "ImageRenderer.h"

const std::string assetsFolderPath = "C:\\Users\\leoqu\\Desktop\\Code\\Project\\Assets\\";

Sprite::Sprite(point2D pos, std::string assetName)
{
	m_pos = pos;
	m_assetPath = assetsFolderPath + assetName + ".bmp";
	std::cout << "created sprite " << m_assetPath << std::endl;
}

bool Sprite::createRenderer(SDL_Renderer* renderer)
{
	ImageRenderer* imageRenderer = new ImageRenderer(renderer, m_assetPath);
	m_renderer = imageRenderer;
	m_size = imageRenderer->getSize();

	return m_renderer->isGenerated();
}