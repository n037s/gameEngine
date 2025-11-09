#include "Font.h"
#include <iostream>


Font::Font()
{
	m_fontPath = "";
	m_policySize = 0;
	m_textAlignement = { Alignement::CENTER, Alignement::CENTER };
	m_font = nullptr;
	m_isGenerated = false;
}

Font::Font(std::string fontPath, float policySize, TextAlignement textAlignement)
{
	m_fontPath = fontPath;
	m_policySize = policySize;
	m_textAlignement = textAlignement;
	loadFont();
}

Font::~Font()
{
	TTF_CloseFont(m_font);
}

void Font::setFontPath(std::string fontPath)
{
	m_fontPath = fontPath;
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
	m_font = TTF_OpenFont(m_fontPath.c_str(), m_policySize);
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