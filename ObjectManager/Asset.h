#pragma once

#include <string>

class Asset
{
public:
	void setAssetID(std::string assetID) { m_assetID = assetID; }
	std::string getAssetID() { return m_assetID; }

private:
	std::string m_assetID{ "" };
};

