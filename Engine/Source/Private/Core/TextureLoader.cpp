#include "Core/TextureLoader.h"

#include "Core/BinaryReader.h"

#include "Core/Texture.h"
#include "Core/Log.h"

#include <cstdint>

namespace Nightbird::Core
{
	std::shared_ptr<Texture> TextureLoader::Load(const std::string& cookedDir, const uuids::uuid& uuid)
	{
		std::string path = cookedDir + "/" + uuids::to_string(uuid) + ".nbtexture";

		BinaryReader reader(path);
		if (!reader.IsValid())
		{
			Log::Error("TextureLoader: Failed to open: " + path);
			return nullptr;
		}

		// Validate Type
		uint8_t type[4] = {};
		reader.ReadRawBytes(type, 4);
		if (type[0] != 'T' || type[1] != 'E' || type[2] != 'X' || type[3] != 'T')
		{
			Log::Error("TextureLoader: Invalid type signature in: " + path);
			return nullptr;
		}

		// Check Version
		uint32_t version = reader.ReadUInt32();
		if (version != 1)
		{
			Log::Error("TextureLoader: Unsupported version: " + std::to_string(version));
			return nullptr;
		}

		// Dimensions
		uint32_t width = reader.ReadUInt32();
		uint32_t height = reader.ReadUInt32();

		// Format
		TextureFormat format = static_cast<TextureFormat>(reader.ReadUInt32());

		// Data size
		uint32_t dataSize = reader.ReadUInt32();

		// Data
		std::vector<uint8_t> data(dataSize);
		reader.ReadRawBytes(data.data(), dataSize);

		return std::make_shared<Texture>(width, height, format, std::move(data));
	}
}
