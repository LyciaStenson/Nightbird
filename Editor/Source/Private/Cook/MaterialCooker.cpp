#include "Cook/MaterialCooker.h"

#include "Cook/BinaryWriter.h"

#include "Core/Material.h"
#include "Core/Texture.h"
#include "Core/Log.h"

namespace Nightbird::Editor
{
	void MaterialCooker::Cook(const Core::Material& material, const uuids::uuid& uuid, const std::filesystem::path& outputDir, Endianness endianness, const std::unordered_map<const Core::Texture*, uuids::uuid>& textureUUIDs)
	{
		std::filesystem::create_directories(outputDir);

		std::filesystem::path outputPath = outputDir / (uuids::to_string(uuid) + ".nbmaterial");

		BinaryWriter writer(outputPath, endianness);

		// Type
		writer.WriteUInt8('M');
		writer.WriteUInt8('A');
		writer.WriteUInt8('T');
		writer.WriteUInt8('L');

		// Version
		writer.WriteUInt32(1);

		// Factors
		writer.WriteFloat(material.baseColorFactor.r);
		writer.WriteFloat(material.baseColorFactor.g);
		writer.WriteFloat(material.baseColorFactor.b);
		writer.WriteFloat(material.baseColorFactor.a);
		writer.WriteFloat(material.metallicFactor);
		writer.WriteFloat(material.roughnessFactor);

		// Flags
		writer.WriteUInt8(material.transparencyEnabled ? 1 : 0);
		writer.WriteUInt8(material.doubleSided ? 1 : 0);

		// Base color texture
		writer.WriteUInt8(material.baseColorTexture ? 1 : 0);
		if (material.baseColorTexture)
		{
			auto it = textureUUIDs.find(material.baseColorTexture.get());
			if (it != textureUUIDs.end())
			{
				auto bytes = it->second.as_bytes();
				writer.WriteRawBytes(reinterpret_cast<const uint8_t*>(bytes.data()), bytes.size());
			}
		}

		// Metallic roughness texture
		writer.WriteUInt8(material.metallicRoughnessTexture ? 1 : 0);
		if (material.metallicRoughnessTexture)
		{
			auto it = textureUUIDs.find(material.metallicRoughnessTexture.get());
			if (it != textureUUIDs.end())
			{
				auto bytes = it->second.as_bytes();
				writer.WriteRawBytes(reinterpret_cast<const uint8_t*>(bytes.data()), bytes.size());
			}
		}

		// Normal texture
		writer.WriteUInt8(material.normalTexture ? 1 : 0);
		if (material.normalTexture)
		{
			auto it = textureUUIDs.find(material.normalTexture.get());
			if (it != textureUUIDs.end())
			{
				auto bytes = it->second.as_bytes();
				writer.WriteRawBytes(reinterpret_cast<const uint8_t*>(bytes.data()), bytes.size());
			}
		}
	}
}