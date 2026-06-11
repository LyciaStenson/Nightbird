#pragma once

#include "Import/TextureImporter.h"

namespace Nightbird::Editor
{
	class StbTextureImporter : public TextureImporter
	{
	public:
		std::string GetName() const override { return "stb_texture"; }
		bool SupportsExtension(const std::string& extension) const override;
		std::shared_ptr<Core::Texture> Load(const AssetInfo& assetInfo) override;
	};
}
