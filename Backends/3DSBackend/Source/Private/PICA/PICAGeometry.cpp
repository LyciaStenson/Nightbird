#include "PICA/PICAGeometry.h"

#include "Core/MeshPrimitive.h"
#include "Core/Vertex.h"

#include <3ds.h>

#include <cstring>

namespace Nightbird::PICA
{
	Geometry::Geometry(const Core::MeshPrimitive& primitive)
	{
		const auto& vertices = primitive.GetVertices();
		const auto& indices = primitive.GetIndices();

		m_VertexCount = static_cast<uint32_t>(vertices.size());
		m_IndexCount = static_cast<uint32_t>(indices.size());

		m_VertexBuffer = linearAlloc(m_VertexCount * sizeof(Vertex));
		Vertex* picaVertices = static_cast<Vertex*>(m_VertexBuffer);
		for (uint32_t i = 0; i < m_VertexCount; i++)
		{
			picaVertices[i].x = vertices[i].position.x;
			picaVertices[i].y = vertices[i].position.y;
			picaVertices[i].z = vertices[i].position.z;

			picaVertices[i].u = vertices[i].baseColorTexCoord.x;
			picaVertices[i].v = 1.0f - vertices[i].baseColorTexCoord.y;
		}

		m_IndexBuffer = linearAlloc(m_IndexCount * sizeof(uint16_t));
		memcpy(m_IndexBuffer, indices.data(), m_IndexCount * sizeof(uint16_t));

		GSPGPU_FlushDataCache(m_VertexBuffer, m_VertexCount * sizeof(Vertex));
		GSPGPU_FlushDataCache(m_IndexBuffer, m_IndexCount * sizeof(uint16_t));
	}

	Geometry::~Geometry()
	{
		if (m_VertexBuffer)
			linearFree(m_VertexBuffer);
		if (m_IndexBuffer)
			linearFree(m_IndexBuffer);
	}

	Geometry::Geometry(Geometry&& other) noexcept
		: m_VertexBuffer(other.m_VertexBuffer), m_IndexBuffer(other.m_IndexBuffer)
		, m_VertexCount(other.m_VertexCount), m_IndexCount(other.m_IndexCount)
	{
		other.m_VertexBuffer = nullptr;
		other.m_IndexBuffer = nullptr;
		other.m_VertexCount = 0;
		other.m_IndexCount = 0;
	}

	Geometry& Geometry::operator=(Geometry&& other) noexcept
	{
		if (this != &other)
		{
			if (m_VertexBuffer)
				linearFree(m_VertexBuffer);
			if (m_IndexBuffer)
				linearFree(m_IndexBuffer);

			m_VertexBuffer = other.m_VertexBuffer;
			m_IndexBuffer = other.m_IndexBuffer;
			m_VertexCount = other.m_VertexCount;
			m_IndexCount = other.m_IndexCount;

			other.m_VertexBuffer = nullptr;
			other.m_IndexBuffer = nullptr;
			other.m_VertexCount = 0;
			other.m_IndexCount = 0;
		}
		return *this;
	}

	void* Geometry::GetVertexBuffer() const
	{
		return m_VertexBuffer;
	}

	void* Geometry::GetIndexBuffer() const
	{
		return m_IndexBuffer;
	}

	uint32_t Geometry::GetVertexCount() const
	{
		return m_VertexCount;
	}

	uint32_t Geometry::GetIndexCount() const
	{
		return m_IndexCount;
	}
}
