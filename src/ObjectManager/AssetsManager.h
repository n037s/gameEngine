#pragma once

#include <string>
#include <map>
#include <functional>
#include <filesystem>
#include <list>

#include "Asset.h"

namespace fs = std::filesystem;

using parsingFunction = std::function<Asset* (const fs::path& assetPath)>;

class AssetsManager
{
public:
	static AssetsManager* getInstance() {
		if (!m_instance)
			m_instance = new AssetsManager();
		return m_instance;
	}
	void Init();

	std::string getAssetsPath() const { return m_assetsFolderPath; }
	std::string getPath(const std::string& assetID) const;
	Asset* getAsset(const std::string& assetID) const;

	void setParserFunction(const std::string& extension, parsingFunction parsingFunc);

	std::list<std::string> getAssetsIDs() const;
	std::list<std::string> getAssets() const { return m_imageIDs; }
	std::list<std::string> getAnimatedAssets() const { return m_animationIDs; }
	std::list<std::string> getFont() const { return m_fontIDs; }

private:
	AssetsManager();
	~AssetsManager() {
		for (auto& pair : m_nameToAsset) {
			delete pair.second;
		}
	};

	void parseFiles();
	Asset* loadAsset(const fs::path& filePath);

	static AssetsManager* m_instance;
	const std::string m_assetsFolderPath = "Assets/";

	std::map<std::string, fs::path> m_nameToPath;
	std::map<std::string, Asset*> m_nameToAsset;
	std::map<std::string, parsingFunction> m_extensionToParsing;
	std::list<std::string> m_imageIDs;
	std::list<std::string> m_animationIDs;
	std::list<std::string> m_fontIDs;

	Asset* m_defaultAsset{nullptr};
};

Asset* parserImage(const fs::path& assetPath);