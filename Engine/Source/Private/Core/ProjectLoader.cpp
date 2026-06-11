#include "Core/ProjectLoader.h"

#include "Core/BinaryReader.h"
#include "Core/Log.h"

#include <array>

namespace Nightbird::Core
{
	ProjectInfo ProjectLoader::Load(const std::string& cookedDir)
	{
		std::string path = cookedDir + "/Project.nbproject";

		BinaryReader reader(path);
		if (!reader.IsValid())
		{
			Log::Error("ProjectLoader: Failed to open: " + path);
			return {};
		}

		// Validate type
		uint8_t type[4] = {};
		reader.ReadRawBytes(type, 4);
		if (type[0] != 'P' || type[1] != 'R' || type[2] != 'O' || type[3] != 'J')
		{
			Log::Error("ProjectLoader: Invalid type signature in: " + path);
			return {};
		}

		// Check Version
		uint32_t version = reader.ReadUInt32();
		if (version != 1)
		{
			Log::Error("ProjectLoader: Unsupported version: " + std::to_string(version));
			return {};
		}

		std::array<uint8_t, 16> mainSceneUUIDBytes;
		reader.ReadRawBytes(mainSceneUUIDBytes.data(), 16);
		return { uuids::uuid(mainSceneUUIDBytes) };
	}
}
