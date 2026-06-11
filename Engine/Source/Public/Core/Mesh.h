#pragma once

#include "Core/Reflection.h"

#include "Core/MeshPrimitive.h"

#include <vector>

namespace Nightbird::Core
{
	class Mesh
	{
	public:
		NB_TYPE_BASE()

		Mesh() = default;
		Mesh(std::vector<MeshPrimitive> primitives);

		const std::vector<MeshPrimitive>& GetPrimitives() const;
		size_t GetPrimitiveCount() const;

	private:
		std::vector<MeshPrimitive> m_Primitives;
	};
}
