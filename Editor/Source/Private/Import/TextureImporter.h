#pragma once

#include "Import/Importer.h"

#include <memory>

namespace Nightbird::Core
{
	class Texture;
}

namespace Nightbird::Editor
{
	struct AssetInfo;

	class TextureImporter : public Importer
	{
	public:
		TextureImporter* AsTextureImporter() override { return this; }

		virtual std::shared_ptr<Core::Texture> Load(const AssetInfo& assetInfo) = 0;
	};
}
