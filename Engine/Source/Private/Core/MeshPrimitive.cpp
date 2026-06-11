#include "Core/MeshPrimitive.h"

namespace Nightbird::Core
{
	MeshPrimitive::MeshPrimitive(std::vector<Vertex> vertices, std::vector<uint16_t> indices, std::shared_ptr<Material> material)
		: m_Vertices(std::move(vertices)), m_Indices(std::move(indices)), m_Material(material)
	{

	}

	const std::vector<Vertex>& MeshPrimitive::GetVertices() const
	{
		return m_Vertices;
	}

	const std::vector<uint16_t>& MeshPrimitive::GetIndices() const
	{
		return m_Indices;
	}

	const std::shared_ptr<Material>& MeshPrimitive::GetMaterial() const
	{
		return m_Material;
	}
}
