#pragma once

#include <SDL3/SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <memory>

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
	Font(const std::string& fontID, float policySize, TextAlignement textAlignement = {Alignement::CENTER, Alignement::CENTER});
	~Font();

	void setFontID(const std::string& fontID);
	void setPolicySize(float policySize);
	void setTextAlignement(const TextAlignement& textAlignement) { m_textAlignement = textAlignement; }
	const std::string& getFontID() const { return m_fontID; }
	float getPolicySize() const { return m_policySize; }
	const TextAlignement& getTextAlignement() const { return m_textAlignement; }

	bool isGenerated() const { return m_isGenerated; }
	TTF_Font* toSDL() const { return m_font; }

private:
	void loadFont();
	bool m_isGenerated;

	TTF_Font* m_font;
	std::string m_fontID;
	float m_policySize;
	TextAlignement m_textAlignement;
};

using FontPtr = std::shared_ptr<Font>;