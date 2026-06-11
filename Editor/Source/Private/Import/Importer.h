#pragma once

#include "Import/AssetInfo.h"

#include <string>
#include <optional>
#include <filesystem>

namespace Nightbird::Editor
{
	class SceneImporter;
	class TextureImporter;
	class CubemapImporter;
	class AudioImporter;

	class Importer
	{
	public:
		virtual ~Importer() = default;
		virtual std::string GetName() const = 0;
		virtual bool SupportsExtension(const std::string& extensions) const = 0;
		
		virtual bool HasEmbeddedAssetInfo() const { return false; }
		virtual std::optional<AssetInfo> ReadEmbeddedAssetInfo(const std::filesystem::path& path) const { return std::nullopt; }
		
		virtual SceneImporter* AsSceneImporter() { return nullptr; }
		virtual TextureImporter* AsTextureImporter() { return nullptr; }
		virtual CubemapImporter* AsCubemapImporter() { return nullptr; }
		virtual AudioImporter* AsAudioImporter() { return nullptr; }
	};
}
