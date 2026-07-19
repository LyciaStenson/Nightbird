#pragma once

#include "Core/SceneObject.h"

namespace Nightbird::Core
{
	class NB_API AmbientLight : public SceneObject
	{
	public:
		NB_TYPE()
		
		using SceneObject::SceneObject;

		glm::vec3 m_Color = glm::vec3(1.0f);
		float m_Intensity = 1.0f;
	};
}
