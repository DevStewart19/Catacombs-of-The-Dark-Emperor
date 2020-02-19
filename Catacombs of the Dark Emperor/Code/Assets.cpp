
#include "Stdafx.h"
#include "Assets.h"

Assets* Assets::instance = nullptr;

sf::Texture& Assets::GetTexture(std::string name)
{
	// Returns an iterator pointing to the found element
	// of the hash table. If this equals textures.end(),
	// it means the item does not exist in the table.
	auto it = textures.find(name);

	if (it == textures.end())
	{
		sf::Texture tex;

		if (!tex.loadFromFile(name))
			OutputDebugStringA("Failed to load player texture.");

		textures.insert(std::make_pair(name, tex));
		return textures[name];
	}

	// it->first is the key (name), and it->second is the value (the texture).
	return it->second;
}