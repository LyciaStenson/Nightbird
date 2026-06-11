#pragma once

#include <uuid.h>

#include <memory>
#include <string>

namespace Nightbird::Core
{
	class AssetManager;
	class Mesh;

	class MeshLoader
	{
	public:
		std::shared_ptr<Mesh> Load(AssetManager& assetManager, const std::string& cookedDir, const uuids::uuid& uuid);
	};
}
