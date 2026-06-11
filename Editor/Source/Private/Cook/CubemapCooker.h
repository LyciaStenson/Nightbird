#pragma once

#include "Cook/Target.h"
#include "Cook/Endianness.h"

#include <uuid.h>

#include <filesystem>
#include <vector>

namespace Nightbird::Core
{
	class AssetManager;
	class Cubemap;
}

namespace Nightbird::Editor
{
	class CubemapCooker
	{
	public:
		void Cook(const Core::Cubemap& cubemap, const uuids::uuid& uuid, const std::filesystem::path& outputDir, Core::AssetManager& assetManager, CookTarget target, Endianness endianness);

	private:
		std::vector<uint8_t> CookRGBA8(const Core::Cubemap& cubemap, Core::AssetManager& assetManager);
	};
}