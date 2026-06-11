#include "Core/SpatialObject.h"

NB_REFLECT(Nightbird::Core::SpatialObject, NB_PARENT(Nightbird::Core::SceneObject), NB_FACTORY(Nightbird::Core::SpatialObject),
	NB_FIELD(m_Transform)
)

namespace Nightbird::Core
{
	glm::mat4 SpatialObject::GetLocalMatrix() const
	{
		return m_Transform.GetLocalMatrix();
	}

	glm::mat4 SpatialObject::GetWorldMatrix() const
	{
		if (m_Parent)
		{
			if (auto* spatialParent = Cast<SpatialObject>(m_Parent))
				return spatialParent->GetWorldMatrix() * GetLocalMatrix();
		}

		return GetLocalMatrix();
	}
}
