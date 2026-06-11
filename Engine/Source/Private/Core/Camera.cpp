#include "Core/Camera.h"

#include <glm/gtc/matrix_transform.hpp>

NB_REFLECT(Nightbird::Core::Camera, NB_PARENT(Nightbird::Core::SpatialObject), NB_FACTORY(Nightbird::Core::Camera),
	NB_FIELD(m_Fov)
)

namespace Nightbird::Core
{
	glm::mat4 Camera::GetViewMatrix() const
	{
		return glm::inverse(GetWorldMatrix());
	}

	glm::mat4 Camera::GetProjectionMatrix(float width, float height) const
	{
#ifdef __WIIU__
		return glm::perspectiveRH_ZO(glm::radians(m_Fov), width / height, 0.001f, 1000.0f);
#else
		return glm::perspective(glm::radians(m_Fov), width / height, 0.001f, 1000.0f);
#endif
	}
}
