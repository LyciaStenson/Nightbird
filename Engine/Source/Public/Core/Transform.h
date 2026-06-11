#pragma once

#include "Core/Reflection.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Nightbird::Core
{
	struct Transform
	{
		NB_TYPE_BASE()
		
		glm::mat4 GetLocalMatrix() const;

		glm::vec3 position = glm::vec3(0.0f);
		glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		glm::vec3 scale = glm::vec3(1.0f);
	};
}
