#include "TextureManager.h"

unordered_map<string, sf::Texture> TextureManager::textureMap;

void TextureManager::LoadTexture(string shortName) {

	string filePath = "images/";
	filePath += shortName + ".png";

	textureMap[shortName].loadFromFile(filePath);

}

sf::Texture& TextureManager::GetTexture(string shortName) {
	if (textureMap.find(shortName) == textureMap.end()) {
		LoadTexture(shortName);
	}

	return textureMap[shortName];
}

void TextureManager::Clear() {
	textureMap.clear();

}