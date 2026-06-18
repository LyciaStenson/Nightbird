#include "Core/CubemapLoader.h"

#include "Core/BinaryReader.h"

#include "Core/Cubemap.h"
#include "Core/Log.h"

namespace Nightbird::Core
{
	std::shared_ptr<Cubemap> CubemapLoader::Load(const std::string& cookedDir, const uuids::uuid& uuid)
	{
		std::string path = cookedDir + "/" + uuids::to_string(uuid) + ".nbcubemap";

		BinaryReader reader(path);
		if (!reader.IsValid())
		{
			Log::Error("CubemapLoader: Failed to open: " + path);
			return nullptr;
		}

		// Validate Type
		uint8_t type[4] = {};
		reader.ReadRawBytes(type, 4);
		if (type[0] != 'C' || type[1] != 'U' || type[2] != 'B' || type[3] != 'E')
		{
			Log::Error("CubemapLoader: Invalid type signature in: " + path);
			return nullptr;
		}

		// Check Version
		uint32_t version = reader.ReadUInt32();
		if (version != 1)
		{
			Log::Error("CubemapLoader: Unsupported version " + std::to_string(version) + " in: " + path);
			return nullptr;
		}

		// Face size
		uint32_t faceSize = reader.ReadUInt32();

		// Data
		uint32_t dataSize = reader.ReadUInt32();
		std::vector<uint8_t> data(dataSize);
		reader.ReadRawBytes(data.data(), dataSize);

		return std::make_shared<Cubemap>(faceSize, std::move(data));
	}
}