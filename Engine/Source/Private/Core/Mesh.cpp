#include "Core/Mesh.h"

NB_REFLECT_NO_FIELDS(Nightbird::Core::Mesh, NB_NO_PARENT, NB_NO_FACTORY)

namespace Nightbird::Core
{
	Mesh::Mesh(std::vector<MeshPrimitive> primitives)
		: m_Primitives(std::move(primitives))
	{

	}

	const std::vector<MeshPrimitive>& Mesh::GetPrimitives() const
	{
		return m_Primitives;
	}

	size_t Mesh::GetPrimitiveCount() const
	{
		return m_Primitives.size();
	}
}
