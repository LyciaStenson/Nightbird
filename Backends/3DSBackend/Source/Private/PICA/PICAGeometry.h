#pragma once

#include <citro3d.h>

#include <cstdint>

namespace Nightbird::Core
{
	class MeshPrimitive;
}

namespace Nightbird::PICA
{
	struct Vertex
	{
		float x, y, z; // position
		float u, v; // texcoords
	};

	class Geometry
	{
	public:
		Geometry() = default;
		Geometry(const Core::MeshPrimitive& primitive);
		~Geometry();

		Geometry(const Geometry&) = delete;
		Geometry& operator=(const Geometry&) = delete;
		Geometry(Geometry&& other) noexcept;
		Geometry& operator=(Geometry&& other) noexcept;

		void* GetVertexBuffer() const;
		void* GetIndexBuffer() const;

		uint32_t GetVertexCount() const;
		uint32_t GetIndexCount() const;

	private:
		void* m_VertexBuffer = nullptr;
		void* m_IndexBuffer = nullptr;

		uint32_t m_VertexCount = 0;
		uint32_t m_IndexCount = 0;
	};
}
