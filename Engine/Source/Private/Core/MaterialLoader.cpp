#include "Core/MaterialLoader.h"

#include "Core/AssetManager.h"
#include "Core/BinaryReader.h"

#include "Core/Material.h"
#include "Core/Texture.h"
#include "Core/Log.h"

#include <array>
#include <cstdint>

namespace Nightbird::Core
{
	std::shared_ptr<Material> MaterialLoader::Load(AssetManager& assetManager, const std::string& cookedDir, const uuids::uuid& uuid)
	{
		std::string path = cookedDir  + "/" + uuids::to_string(uuid) + ".nbmaterial";

		BinaryReader reader(path);
		if (!reader.IsValid())
		{
			Log::Error("MaterialLoader: Failed to open: " + path);
			return nullptr;
		}

		// Validate type		
		uint8_t type[4] = {};
		reader.ReadRawBytes(type, 4);
		if (type[0] != 'M' || type[1] != 'A' || type[2] != 'T' || type[3] != 'L')
		{
			Log::Error("MaterialLoader: Invalid type signature in: " + path);
			return nullptr;
		}

		// Check Version
		uint32_t version = reader.ReadUInt32();
		if (version != 1)
		{
			Log::Error("MaterialLoader: Unsupported version: " + std::to_string(version));
			return nullptr;
		}

		auto material = std::make_shared<Material>();

		material->baseColorFactor.r = reader.ReadFloat();
		material->baseColorFactor.g= reader.ReadFloat();
		material->baseColorFactor.b = reader.ReadFloat();
		material->baseColorFactor.a = reader.ReadFloat();
		material->metallicFactor = reader.ReadFloat();
		material->roughnessFactor = reader.ReadFloat();
		material->transparencyEnabled = reader.ReadUInt8();
		material->doubleSided = reader.ReadUInt8();

		uint8_t hasBaseColor = reader.ReadUInt8();
		if (hasBaseColor)
		{
			std::array<uint8_t, 16> uuidBytes;
			reader.ReadRawBytes(uuidBytes.data(), 16);
			uuids::uuid textureUUID(uuidBytes);
			material->baseColorTexture = assetManager.Load<Texture>(textureUUID).lock();
		}

		uint8_t hasMetallicRoughness = reader.ReadUInt8();
		if (hasMetallicRoughness)
		{
			std::array<uint8_t, 16> uuidBytes;
			reader.ReadRawBytes(uuidBytes.data(), 16);
			uuids::uuid textureUUID(uuidBytes);
			material->metallicRoughnessTexture = assetManager.Load<Texture>(textureUUID).lock();
		}

		uint8_t hasNormal = reader.ReadUInt8();
		if (hasNormal)
		{
			std::array<uint8_t, 16> uuidBytes;
			reader.ReadRawBytes(uuidBytes.data(), 16);
			uuids::uuid textureUUID(uuidBytes);
			material->normalTexture = assetManager.Load<Texture>(textureUUID).lock();
		}

		return material;
	}
}
