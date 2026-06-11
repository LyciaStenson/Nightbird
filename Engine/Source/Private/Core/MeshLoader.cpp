#include "Core/MeshLoader.h"

#include "Core/AssetManager.h"
#include "Core/BinaryReader.h"

#include "Core/Mesh.h"
#include "Core/MeshPrimitive.h"
#include "Core/Log.h"

#include <array>
#include <cstdint>

namespace Nightbird::Core
{
	std::shared_ptr<Mesh> MeshLoader::Load(AssetManager& assetManager, const std::string& cookedDir, const uuids::uuid& uuid)
	{
		std::string path = cookedDir + "/" + uuids::to_string(uuid) + ".nbmesh";
		
		BinaryReader reader(path);
		if (!reader.IsValid())
		{
			Log::Error("MeshLoader: Failed to open: " + path);
			return nullptr;
		}

		// Validate type		
		uint8_t type[4] = {};
		reader.ReadRawBytes(type, 4);
		if (type[0] != 'M' || type[1] != 'E' || type[2] != 'S' || type[3] != 'H')
		{
			Log::Error("MeshLoader: Invalid type signature in: " + path);
			return nullptr;
		}

		// Check Version
		uint32_t version = reader.ReadUInt32();
		if (version != 1)
		{
			Log::Error("MeshLoader: Unsupported version: " + std::to_string(version));
			return nullptr;
		}

		uint32_t primitiveCount = reader.ReadUInt32();

		std::vector<MeshPrimitive> primitives;
		primitives.reserve(primitiveCount);

		for (uint32_t i = 0; i < primitiveCount; i++)
		{
			uint32_t vertexCount = reader.ReadUInt32();
			auto vertices = reader.ReadVertices(vertexCount);

			uint32_t indexCount = reader.ReadUInt32();
			auto indices = reader.ReadIndices(indexCount);

			uint8_t hasMaterial = reader.ReadUInt8();
			std::shared_ptr<Material> material;
			if (hasMaterial)
			{
				std::array<uint8_t, 16> uuidBytes;
				reader.ReadRawBytes(uuidBytes.data(), 16);
				uuids::uuid materialUUID(uuidBytes);
				material = assetManager.Load<Material>(materialUUID).lock();
			}

			primitives.emplace_back(std::move(vertices), std::move(indices), material);
		}

		return std::make_shared<Mesh>(std::move(primitives));
	}
}
