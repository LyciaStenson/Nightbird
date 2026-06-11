#include "Cook/MeshCooker.h"

#include "Cook/BinaryWriter.h"

#include "Core/Mesh.h"
#include "Core/Material.h"
#include "Core/MeshPrimitive.h"
#include "Core/Log.h"

namespace Nightbird::Editor
{
	void MeshCooker::Cook(const Core::Mesh& mesh, const uuids::uuid& uuid,
		const std::filesystem::path& outputDir, Endianness endianness,
		const std::unordered_map<const Core::Material*, uuids::uuid>& materialUUIDs)
	{
		std::filesystem::create_directory(outputDir);
		
		std::filesystem::path outputPath = outputDir / (uuids::to_string(uuid) + ".nbmesh");

		BinaryWriter writer(outputPath, endianness);

		// Type
		writer.WriteUInt8('M');
		writer.WriteUInt8('E');
		writer.WriteUInt8('S');
		writer.WriteUInt8('H');

		// Version
		writer.WriteUInt32(1);

		writer.WriteUInt32(static_cast<uint32_t>(mesh.GetPrimitiveCount()));

		for (size_t i = 0; i < mesh.GetPrimitiveCount(); ++i)
		{
			const Core::MeshPrimitive& primitive = mesh.GetPrimitives()[i];

			const auto& vertices = primitive.GetVertices();
			const auto& indices = primitive.GetIndices();

			// Vertex count
			writer.WriteUInt32(static_cast<uint32_t>(vertices.size()));
			// Vertex data
			writer.WriteVertices(vertices);
			
			// Index count
			writer.WriteUInt32(static_cast<uint32_t>(indices.size()));
			// Index data
			writer.WriteIndices(indices);
			
			// Material flag
			const auto& material = primitive.GetMaterial();
			writer.WriteUInt8(material ? 1 : 0);

			// Material UUID
			if (material)
			{
				auto it = materialUUIDs.find(material.get());
				if (it != materialUUIDs.end())
				{
					auto bytes = it->second.as_bytes();
					writer.WriteRawBytes(reinterpret_cast<const uint8_t*>(bytes.data()), bytes.size());
				}
			}
		}

		Core::Log::Info("Cooked mesh: " + outputPath.string());
	}
}