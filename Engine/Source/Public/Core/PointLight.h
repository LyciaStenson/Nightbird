#pragma once

#include "Core/SpatialObject.h"

namespace Nightbird::Core
{
	class PointLight : public SpatialObject
	{
	public:
		NB_TYPE()
			
		using SpatialObject::SpatialObject;

		glm::vec3 m_Color = glm::vec3(1.0f);
		float m_Intensity = 1.0f;
		float m_Radius = 10.0f;
	};
}
