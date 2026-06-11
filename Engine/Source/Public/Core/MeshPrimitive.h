#pragma once

#include "Core/Vertex.h"
#include "Core/Material.h"

#include <vector>
#include <memory>

namespace Nightbird::Core
{
	class MeshPrimitive
	{
	public:
		MeshPrimitive(std::vector<Vertex> vertices, std::vector<uint16_t> indices, std::shared_ptr<Material> material);

		const std::vector<Vertex>& GetVertices() const;
		const std::vector<uint16_t>& GetIndices() const;
		const std::shared_ptr<Material>& GetMaterial() const;

	private:
		std::vector<Vertex> m_Vertices;
		std::vector<uint16_t> m_Indices;
		std::shared_ptr<Material> m_Material;
	};
}