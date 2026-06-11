#include "Import/TextCubemapImporter.h"

#include "Core/Cubemap.h"
#include "Core/Texture.h"
#include "Core/Log.h"

#include <toml.hpp>

#include <memory>
#include <array>

namespace Nightbird::Editor
{
	static constexpr const char* s_FaceKeys[6] = { "pos_x", "neg_x", "pos_y", "neg_y", "pos_z", "neg_z" };

	bool TextCubemapImporter::SupportsExtension(const std::string& extension) const
	{
		return extension == ".ntcubemap";
	}

	std::optional<AssetInfo> TextCubemapImporter::ReadEmbeddedAssetInfo(const std::filesystem::path& path) const
	{
		if (!std::filesystem::exists(path))
		{
			Core::Log::Error("TextCubemapImporter: File not found: " + path.string());
			return std::nullopt;
		}

		toml::parse_result tomlResult = toml::parse_file(path.string());
		if (!tomlResult)
		{
			Core::Log::Error("TextCubemapImporter: Failed to parse: " + path.string());
			return std::nullopt;
		}

		toml::table& document = tomlResult.table();
		
		std::string cubemapUUIDString = document["cubemap"]["uuid"].value_or(std::string{});
		auto cubemapUUID = uuids::uuid::from_string(cubemapUUIDString);
		if (!cubemapUUID)
		{
			Core::Log::Error("TextCubemapImporter: Invalid cubemap UUID in: " + path.string());
			return std::nullopt;
		}

		AssetInfo assetInfo;
		assetInfo.uuid = *cubemapUUID;
		assetInfo.importer = GetName();
		assetInfo.path = path;

		auto facesTable = document["faces"];
		for (const char* key : s_FaceKeys)
		{
			auto value = facesTable[key].value<std::string>();
			if (value)
				assetInfo.tags[key] = *value;
			else
				assetInfo.tags[key] = {};
		}

		return assetInfo;
	}
	
	CubemapReadResult TextCubemapImporter::Load(const AssetInfo& assetInfo, ImportManager* importManager)
	{
		CubemapReadResult result;

		static constexpr const char* faceNames[6] = {"+X", "-X", "+Y", "-Y", "+Z", "-Z"};

		std::array<uuids::uuid, 6> faceUUIDs;
		for (int i = 0; i < 6; ++i)
		{
			auto it = assetInfo.tags.find(s_FaceKeys[i]);
			if (it == assetInfo.tags.end() || it->second.empty())
			{
				Core::Log::Error("TextCubemapImporter: Missing face " + std::string(faceNames[i]) + " in cubemap: " + assetInfo.path.string());
				return result;
			}

			auto uuid = uuids::uuid::from_string(it->second);
			if (!uuid)
			{
				Core::Log::Error("TextCubemapImporter: Invalid face UUID for " + std::string(faceNames[i]));
				return result;
			}

			faceUUIDs[i] = *uuid;
		}

		std::array<std::shared_ptr<Core::Texture>, 6> textures;
		for (int i = 0; i < 6; ++i)
		{
			auto texture = importManager->Load<Core::Texture>(faceUUIDs[i]).lock();
			if (!texture)
			{
				Core::Log::Error("TextCubemapImporter: Failed to load face " + std::string(faceNames[i]));
				return result;
			}
			textures[i] = texture;
		}

		for (int i = 0; i < 6; ++i)
		{
			if (textures[i]->GetWidth() != textures[i]->GetHeight())
			{
				Core::Log::Error("TextCubemapImporter: Face " + std::string(faceNames[i]) + " is not square (" + std::to_string(textures[i]->GetWidth()) + "x" + std::to_string(textures[i]->GetHeight()) + ")");
				return result;
			}
		}

		uint32_t faceSize = textures[0]->GetWidth();
		for (int i = 1; i < 6; ++i)
		{
			if (textures[i]->GetWidth() != faceSize)
			{
				Core::Log::Error("TextCubemapImporter: Face " + std::string(faceNames[i]) + " size (" + std::to_string(textures[i]->GetWidth()) + ") does not match +X face size (" + std::to_string(faceSize) + ")");
				return {};
			}
		}

		std::vector<uint8_t> data;
		data.reserve(faceSize * faceSize * 4 * 6);
		for (int i = 0; i < 6; ++i)
		{
			const auto& faceData = textures[i]->GetData();
			data.insert(data.end(), faceData.begin(), faceData.end());
		}

		result.uuid = assetInfo.uuid;
		result.cubemap = std::make_shared<Core::Cubemap>(faceSize, std::move(data));

		return result;
	}
}
