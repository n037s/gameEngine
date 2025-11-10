#include "Font.h"

#include <iostream>

#include "AssetsManager.h"


Font::Font()
{
	m_fontID = "";
	m_policySize = 0;
	m_textAlignement = { Alignement::CENTER, Alignement::CENTER };
	m_font = nullptr;
	m_isGenerated = false;
}

Font::Font(std::string fontID, float policySize, TextAlignement textAlignement)
{
	m_fontID = fontID;
	m_policySize = policySize;
	m_textAlignement = textAlignement;
	loadFont();
}

Font::~Font()
{
	TTF_CloseFont(m_font);
}

void Font::setFontID(std::string fontID)
{
	m_fontID = fontID;
	loadFont();
}

void Font::setPolicySize(float policySize)
{
	m_policySize = policySize;
	loadFont();
}

void Font::loadFont()
{
	TTF_CloseFont(m_font);
	m_font = TTF_OpenFont(AssetsManager::getInstance()->getPath(m_fontID).c_str(), m_policySize);
	if (!m_font)
	{
		m_isGenerated = false;
		std::cout << "Error loading font: " << SDL_GetError() << '\n';
	}
	else
	{
		m_isGenerated = true;
	}
}