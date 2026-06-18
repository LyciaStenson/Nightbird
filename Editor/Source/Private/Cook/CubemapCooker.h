#pragma once

#include "Cook/Target.h"
#include "Cook/Endianness.h"
#include "Import/AssetInfo.h"

#include <uuid.h>

#include <filesystem>
#include <vector>

namespace Nightbird::Core
{
	class AssetManager;
}

namespace Nightbird::Editor
{
	class CubemapCooker
	{
	public:
		void Cook(const AssetInfo& assetInfo, const std::filesystem::path& outputDir, Core::AssetManager& assetManager, CookTarget target, Endianness endianness);

	private:
		std::vector<uint8_t> CookRGBA8(const AssetInfo& assetInfo, Core::AssetManager& assetManager);
	};
}
