#pragma once

#include "Core/SceneObject.h"
#include "Core/Transform.h"

namespace Nightbird::Core
{
	class NB_API SpatialObject : public SceneObject
	{
	public:
		NB_TYPE()

		using SceneObject::SceneObject;
		~SpatialObject() override = default;

		glm::mat4 GetLocalMatrix() const;
		glm::mat4 GetWorldMatrix() const;

		Transform m_Transform;
	};
}
