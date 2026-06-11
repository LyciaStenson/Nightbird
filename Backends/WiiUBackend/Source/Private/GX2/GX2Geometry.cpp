#include "GX2/GX2Geometry.h"

#include "Core/MeshPrimitive.h"
#include "Core/Vertex.h"

#include <gx2r/draw.h>

#include <vector>
#include <cstring>

namespace Nightbird::GX2
{
	Geometry::Geometry(const Core::MeshPrimitive& primitive)
	{
		CreatePositionBuffer(primitive);
		CreateIndexBuffer(primitive);
		CreateTexCoordBuffer(primitive);

		m_IndexCount = static_cast<uint32_t>(primitive.GetIndices().size());
	}

	Geometry::~Geometry()
	{
		if (m_PositionBuffer.buffer)
			GX2RDestroyBufferEx(&m_PositionBuffer, GX2R_RESOURCE_BIND_NONE);
		if (m_IndexBuffer.buffer)
			GX2RDestroyBufferEx(&m_IndexBuffer, GX2R_RESOURCE_BIND_NONE);
		if (m_TexCoordBuffer.buffer)
			GX2RDestroyBufferEx(&m_TexCoordBuffer, GX2R_RESOURCE_BIND_NONE);
	}

	Geometry::Geometry(Geometry&& other) noexcept
	: m_PositionBuffer(other.m_PositionBuffer), m_IndexBuffer(other.m_IndexBuffer), m_TexCoordBuffer(other.m_TexCoordBuffer), m_IndexCount(other.m_IndexCount)
	{
		other.m_PositionBuffer = {};
		other.m_IndexBuffer = {};
		other.m_TexCoordBuffer = {};
		other.m_IndexCount = 0;
	}

	Geometry& Geometry::operator=(Geometry&& other) noexcept
	{
		if (this != &other)
		{
			if (m_PositionBuffer.buffer)
				GX2RDestroyBufferEx(&m_PositionBuffer, GX2R_RESOURCE_BIND_NONE);
			if (m_IndexBuffer.buffer)
				GX2RDestroyBufferEx(&m_IndexBuffer, GX2R_RESOURCE_BIND_NONE);
			if (m_TexCoordBuffer.buffer)
				GX2RDestroyBufferEx(&m_TexCoordBuffer, GX2R_RESOURCE_BIND_NONE);

			m_PositionBuffer = other.m_PositionBuffer;
			m_IndexBuffer = other.m_IndexBuffer;
			m_TexCoordBuffer = other.m_TexCoordBuffer;
			m_IndexCount = other.m_IndexCount;

			other.m_PositionBuffer = {};
			other.m_IndexBuffer = {};
			other.m_TexCoordBuffer = {};
			other.m_IndexCount = 0;
		}
		return *this;
	}

	GX2RBuffer& Geometry::GetPositionBuffer()
	{
		return m_PositionBuffer;
	}

	GX2RBuffer& Geometry::GetIndexBuffer()
	{
		return m_IndexBuffer;
	}

	GX2RBuffer& Geometry::GetTexCoordBuffer()
	{
		return m_TexCoordBuffer;
	}

	uint32_t Geometry::GetIndexCount() const
	{
		return m_IndexCount;
	}

	void Geometry::CreatePositionBuffer(const Core::MeshPrimitive& primitive)
	{
		const auto& vertices = primitive.GetVertices();

		std::vector<float> positions;
		positions.reserve(vertices.size() * 4);
		for (const auto& vertex : vertices)
		{
			positions.push_back(vertex.position.x);
			positions.push_back(vertex.position.y);
			positions.push_back(vertex.position.z);
			positions.push_back(1.0f);
		}

		m_PositionBuffer.flags = GX2R_RESOURCE_BIND_VERTEX_BUFFER |
								 GX2R_RESOURCE_USAGE_CPU_READ |
								 GX2R_RESOURCE_USAGE_CPU_WRITE |
								 GX2R_RESOURCE_USAGE_GPU_READ;
		m_PositionBuffer.elemSize = 4 * sizeof(float);
		m_PositionBuffer.elemCount = static_cast<uint32_t>(vertices.size());
		GX2RCreateBuffer(&m_PositionBuffer);

		void* buffer = GX2RLockBufferEx(&m_PositionBuffer, GX2R_RESOURCE_BIND_NONE);
		memcpy(buffer, positions.data(), m_PositionBuffer.elemSize * m_PositionBuffer.elemCount);
		GX2RUnlockBufferEx(&m_PositionBuffer, GX2R_RESOURCE_BIND_NONE);
	}

	void Geometry::CreateIndexBuffer(const Core::MeshPrimitive& primitive)
	{
		const auto& indices = primitive.GetIndices();

		m_IndexBuffer.flags = GX2R_RESOURCE_BIND_INDEX_BUFFER |
								 GX2R_RESOURCE_USAGE_CPU_READ |
								 GX2R_RESOURCE_USAGE_CPU_WRITE |
								 GX2R_RESOURCE_USAGE_GPU_READ;
		m_IndexBuffer.elemSize = sizeof(uint16_t);
		m_IndexBuffer.elemCount = static_cast<uint32_t>(indices.size());
		GX2RCreateBuffer(&m_IndexBuffer);

		void* buffer = GX2RLockBufferEx(&m_IndexBuffer, GX2R_RESOURCE_BIND_NONE);
		memcpy(buffer, indices.data(), m_IndexBuffer.elemSize * m_IndexBuffer.elemCount);
		GX2RUnlockBufferEx(&m_IndexBuffer, GX2R_RESOURCE_BIND_NONE);
	}

	void Geometry::CreateTexCoordBuffer(const Core::MeshPrimitive& primitive)
	{
		const auto& vertices = primitive.GetVertices();

		std::vector<float> texCoords;
		texCoords.reserve(vertices.size() * 2);
		for (const auto& vertex : vertices)
		{
			texCoords.push_back(vertex.baseColorTexCoord.x);
			texCoords.push_back(vertex.baseColorTexCoord.y);
		}

		m_TexCoordBuffer.flags = GX2R_RESOURCE_BIND_VERTEX_BUFFER |
								 GX2R_RESOURCE_USAGE_CPU_READ |
								 GX2R_RESOURCE_USAGE_CPU_WRITE |
								 GX2R_RESOURCE_USAGE_GPU_READ;
		m_TexCoordBuffer.elemSize = 2 * sizeof(float);
		m_TexCoordBuffer.elemCount = static_cast<uint32_t>(vertices.size());
		GX2RCreateBuffer(&m_TexCoordBuffer);

		void* buffer = GX2RLockBufferEx(&m_TexCoordBuffer, GX2R_RESOURCE_BIND_NONE);
		memcpy(buffer, texCoords.data(), m_TexCoordBuffer.elemSize * m_TexCoordBuffer.elemCount);
		GX2RUnlockBufferEx(&m_TexCoordBuffer, GX2R_RESOURCE_BIND_NONE);
	}
}
