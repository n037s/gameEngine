#include "AssetsManager.h"

#include <iostream>

#include "SDL3/SDL.h"
#include <SDL3_image/SDL_image.h>

#include "Image.h"

AssetsManager* AssetsManager::m_instance = nullptr;

AssetsManager::AssetsManager() 
{ 
    m_nameToPath = std::map<std::string, fs::path>();
    Init(); 
};

void AssetsManager::Init()
{
    // Let's add basic parsing function
    setParserFunction(".bmp", parserImage);
    setParserFunction(".gif", parserImage);
    setParserFunction(".jpeg", parserImage);
    setParserFunction(".png", parserImage);
    setParserFunction(".lbm", parserImage);
    setParserFunction(".pcx", parserImage);
    setParserFunction(".xcf", parserImage);
    setParserFunction(".xpm", parserImage);
    
	// let's parse folder Assets
    if (fs::exists(m_assetsFolderPath)) {
        if (fs::is_directory(m_assetsFolderPath)) {
            parseFiles();
        }
        else
        {
            std::cerr << "Error: Asset folder is not a directory. " << m_assetsFolderPath << std::endl;
        }
    }
    else
    {
        std::cout << "Error: Asset folder does not exist. " << m_assetsFolderPath << std::endl;
    }
}

void AssetsManager::parseFiles()
{
    for (const auto& entry : fs::recursive_directory_iterator(m_assetsFolderPath))
    {
        if (fs::is_regular_file(entry.status()))
        {
            fs::path filePath = entry.path();
            fs::path relative = fs::relative(filePath, m_assetsFolderPath).replace_extension();

            std::string assetID = relative.string();
            std::replace(assetID.begin(), assetID.end(), '\\', '/');

            m_nameToPath[assetID] = filePath;

            Asset* asset = loadAsset(filePath);
            if (asset)
            {
                m_nameToAsset[assetID] = asset;
            }
        }
    }

    std::cout << "Parsed assets : " << std::endl;
    for (auto it : m_nameToAsset)
    {
        std::cout << "item - " << it.first << std::endl;
    }
}

Asset* AssetsManager::loadAsset(fs::path filePath)
{
    Asset* result = nullptr;

    std::string extension = filePath.extension().string();

    if (m_extensionToParsing.find(extension) != m_extensionToParsing.end())
    {
        result = m_extensionToParsing[extension](filePath);
    }
    else
    {
        //std::cout << "No implementation for asset type " << extension << std::endl;
    }

    return result;
}

std::string AssetsManager::getPath(std::string assetID)
{
    return m_nameToPath[assetID].string();
}

Asset* AssetsManager::getAsset(std::string assetID)
{
    Asset* result = nullptr;
    if (m_nameToAsset.find(assetID) != m_nameToAsset.end())
        result = m_nameToAsset[assetID];
    return result;
}

void AssetsManager::setParserFunction(std::string extension, parsingFunction parsingFunc)
{
    m_extensionToParsing[extension] = parsingFunc;
}



// Basic parser function definition

Asset* parserImage(fs::path assetPath)
{
    Asset* result = nullptr;

    SDL_Surface* surface = IMG_Load(assetPath.string().c_str());
    if (!surface) {
        std::cout << "Failed to load BMP image : "
            << assetPath.string() << ":\n  " << SDL_GetError() << '\n';
        result = new Image(nullptr);
    }
    else
    {
        result = new Image(surface);
        static_cast<Image*>(result)->setpath(assetPath.string());
    }
    return result;
}