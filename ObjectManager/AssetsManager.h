#pragma once

#include <string>
#include <map>
#include <functional>
#include <filesystem>

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
	const std::string m_assetsFolderPath = "C:\\Users\\leoqu\\Desktop\\Code\\Project\\Assets\\";

	std::map<std::string, fs::path> m_nameToPath;
	std::map<std::string, Asset*> m_nameToAsset;
	std::map<std::string, parsingFunction> m_extensionToParsing;
};

Asset* parserImage(const fs::path& assetPath);