#pragma once

#include <string>

class Asset
{
public:
	virtual ~Asset() = default;
	void setAssetID(const std::string& assetID) { m_assetID = assetID; }
	std::string getAssetID() const { return m_assetID; }

private:
	std::string m_assetID{ "" };
};



