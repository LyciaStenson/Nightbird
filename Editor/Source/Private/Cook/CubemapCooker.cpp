#include "Cook/CubemapCooker.h"

#include "Cook/BinaryWriter.h"

#include "Core/AssetManager.h"
#include "Core/Cubemap.h"
#include "Core/Texture.h"
#include "Core/Log.h"

namespace Nightbird::Editor
{
	static constexpr const char* s_FaceKeys[6] = { "pos_x", "neg_x", "pos_y", "neg_y", "pos_z", "neg_z" };
	static constexpr const char* s_FaceNames[6] = { "+X", "-X", "+Y", "-Y", "+Z", "-Z" };
	
	void CubemapCooker::Cook(const AssetInfo& assetInfo, const std::filesystem::path& outputDir, Core::AssetManager& assetManager, CookTarget target, Endianness endianness)
	{
		std::filesystem::create_directories(outputDir);
		std::filesystem::path outputPath = outputDir / (uuids::to_string(assetInfo.uuid) + ".nbcubemap");

		std::vector<uint8_t> data;

		switch (target)
		{
		case CookTarget::Desktop:
			data = CookRGBA8(assetInfo, assetManager);
			break;
		default:
			data = CookRGBA8(assetInfo, assetManager);
			break;
		}

		if (data.empty())
		{
			Core::Log::Error("CubemapCooker: Failed to cook: " + outputPath.string());
			return;
		}

		uint32_t faceDataSize = static_cast<uint32_t>(data.size()) / 6;
		uint32_t faceSize = static_cast<uint32_t>(std::sqrt(faceDataSize / 4));

		BinaryWriter writer(outputPath, endianness);

		// Type signature
		writer.WriteUInt8('C');
		writer.WriteUInt8('U');
		writer.WriteUInt8('B');
		writer.WriteUInt8('E');

		// Version
		writer.WriteUInt32(1);

		// Face size
		writer.WriteUInt32(faceSize);

		// Total data size
		writer.WriteUInt32(static_cast<uint32_t>(data.size()));

		writer.WriteRawBytes(data.data(), data.size());

		Core::Log::Info("CubemapCooker: Cooked cubemap: " + outputPath.string());
	}

	std::vector<uint8_t> CubemapCooker::CookRGBA8(const AssetInfo& assetInfo, Core::AssetManager& assetManager)
	{
		std::array<std::shared_ptr<Core::Texture>, 6> textures;
		for (int i = 0; i < 6; ++i)
		{
			auto it = assetInfo.tags.find(s_FaceKeys[i]);
			if (it == assetInfo.tags.end() || it->second.empty())
			{
				Core::Log::Error("CubemapCooker: Face " + std::string(s_FaceNames[i]) + " UUID is missing");
				return {};
			}

			auto faceUUID = uuids::uuid::from_string(it->second);
			if (!faceUUID)
			{
				Core::Log::Error("CubemapCooker: Face " + std::string(s_FaceNames[i]) + " UUID is invalid");
				return {};
			}
			
			auto texture = assetManager.Load<Core::Texture>(*faceUUID).lock();
			if (!texture)
			{
				Core::Log::Error("CubemapCooker: Failed to load face: " + std::string(s_FaceNames[i]));
				return {};
			}

			textures[i] = texture;
		}

		for (int i = 0; i < 6; ++i)
		{
			if (textures[i]->GetWidth() != textures[i]->GetHeight())
			{
				Core::Log::Error("CubemapCooker: Face " + std::string(s_FaceNames[i]) + " is not square (" + std::to_string(textures[i]->GetWidth()) + "x" + std::to_string(textures[i]->GetHeight()) + ")");
				return {};
			}
		}

		uint32_t faceSize = textures[0]->GetWidth();
		for (int i = 1; i < 6; ++i)
		{
			if (textures[i]->GetWidth() != faceSize)
			{
				Core::Log::Error("CubemapCooker: Face " + std::string(s_FaceNames[i]) + " size (" + std::to_string(textures[i]->GetWidth()) + ") does not match +X face size (" + std::to_string(faceSize) + ")");
				return {};
			}
		}

		std::vector<uint8_t> result;
		result.reserve(faceSize * faceSize * 4 * 6);

		for (int i = 0; i < 6; ++i)
		{
			const auto& faceData = textures[i]->GetData();
			result.insert(result.end(), faceData.begin(), faceData.end());
		}

		return result;
	}
}
