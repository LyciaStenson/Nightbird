#pragma once

#include <uuid.h>

#include <memory>
#include <string>

namespace Nightbird::Core
{
	class AssetManager;
	struct Material;

	class MaterialLoader
	{
	public:
		std::shared_ptr<Material> Load(AssetManager& assetManager, const std::string& cookedDir, const uuids::uuid& uuid);
	};
}
