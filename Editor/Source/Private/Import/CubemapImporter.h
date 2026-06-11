#pragma once

#include "Import/CubemapReadResult.h"
#include "Import/ImportManager.h"
#include "Import/Importer.h"

#include <memory>

namespace Nightbird::Core
{
	class SceneObject;
}

namespace Nightbird::Editor
{
	struct AssetInfo;

	class CubemapImporter : public Importer
	{
	public:
		CubemapImporter* AsCubemapImporter() override { return this; }

		virtual CubemapReadResult Load(const AssetInfo& assetInfo, ImportManager* importManager) = 0;
	};
}
