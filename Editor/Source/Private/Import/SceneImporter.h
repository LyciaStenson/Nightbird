#pragma once

#include "Core/SceneReadResult.h"
#include "Core/AssetManager.h"
#include "Import/Importer.h"

#include <memory>

namespace Nightbird::Core
{
	class SceneObject;
}

namespace Nightbird::Editor
{
	struct AssetInfo;

	class SceneImporter : public Importer
	{
	public:
		SceneImporter* AsSceneImporter() override { return this; }

		virtual Core::SceneReadResult Load(const AssetInfo& assetInfo, Core::AssetManager* assetManager = nullptr) = 0;
	};
}
