#include "Import/TextSceneImporter.h"

#include "Scene/TextSceneReader.h"

#include "Core/Log.h"

#include <toml.hpp>

namespace Nightbird::Editor
{
	bool TextSceneImporter::SupportsExtension(const std::string& extension) const
	{
		return extension == ".ntscene";
	}

	std::optional<AssetInfo> TextSceneImporter::ReadEmbeddedAssetInfo(const std::filesystem::path& path) const
	{
		if (!std::filesystem::exists(path))
		{
			Core::Log::Error("TextSceneImporter: File not found: " + path.string());
			return std::nullopt;
		}

		toml::parse_result tomlResult = toml::parse_file(path.string());
		if (!tomlResult)
		{
			Core::Log::Error("TextSceneImporter: Failed to parse: " + path.string());
			return std::nullopt;
		}

		toml::table& document = tomlResult.table();

		std::string sceneName = document["scene"]["name"].value_or(std::string{});
		std::string sceneUUIDString = document["scene"]["uuid"].value_or(std::string{});
		auto sceneUUID = uuids::uuid::from_string(sceneUUIDString);
		if (!sceneUUID)
		{
			Core::Log::Error("TextSceneImporter: Invalid scene UUID in: " + path.string());
			return std::nullopt;
		}

		AssetInfo assetInfo;
		assetInfo.uuid = *sceneUUID;
		assetInfo.importer = GetName();
		assetInfo.path = path;

		return assetInfo;
	}

	Core::SceneReadResult TextSceneImporter::Load(const AssetInfo& assetInfo, Core::AssetManager* assetManager)
	{
		TextSceneReader reader;
		return reader.Read(assetInfo.path);
	}
}
