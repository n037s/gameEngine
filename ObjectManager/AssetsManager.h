#pragma once

#include <string>
#include <map>
#include <functional>
#include <filesystem>

#include "Asset.h"

namespace fs = std::filesystem;

using parsingFunction = std::function<Asset* (fs::path assetPath)>;

class AssetsManager
{
public:
	static AssetsManager* getInstance() {
		if (!m_instance)
			m_instance = new AssetsManager();
		return m_instance;
	}
	void Init();

	std::string getAssetsPath() { return m_assetsFolderPath; }
	std::string getPath(std::string assetID);
	Asset* getAsset(std::string assetID);

	void setParserFunction(std::string extension, parsingFunction);

private:
	AssetsManager();
	~AssetsManager() {};

	void parseFiles();
	Asset* loadAsset(fs::path filePath);

	static AssetsManager* m_instance;
	const std::string m_assetsFolderPath = "C:\\Users\\leoqu\\Desktop\\Code\\Project\\Assets\\";

	std::map<std::string, fs::path> m_nameToPath;
	std::map<std::string, Asset*> m_nameToAsset;
	std::map<std::string, parsingFunction> m_extensionToParsing;
};


Asset* parserImage(fs::path assetPath);