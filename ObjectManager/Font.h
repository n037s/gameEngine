#pragma once

#include <SDL3/SDL.h>
#include <SDL_ttf.h>
#include <string>

#include "Asset.h"

enum Alignement
{
	LOW = 0,
	CENTER, 
	HIGH
};

struct TextAlignement
{
	Alignement horizontalAlignement;
	Alignement verticalAlignement;
};

class Font
{
public:
	Font();
	Font(std::string m_fontID, float policySize, TextAlignement textAlignement = {Alignement::CENTER, Alignement::CENTER});
	~Font();

	void setFontID(std::string fontID);
	void setPolicySize(float policySize);
	void setTextAlignement(TextAlignement textAlignement) { m_textAlignement = textAlignement; }
	TextAlignement getTextAlignement() { return m_textAlignement; }

	bool isGenerated() { return m_isGenerated; }
	TTF_Font* toSDL() { return m_font; }

private:
	void loadFont();
	bool m_isGenerated;

	TTF_Font* m_font;
	std::string m_fontID;
	float m_policySize;
	TextAlignement m_textAlignement;
};

