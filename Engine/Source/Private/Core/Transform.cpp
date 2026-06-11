#include "Core/Transform.h"

#include "Core/Log.h"

NB_REFLECT(Nightbird::Core::Transform, NB_NO_PARENT, NB_FACTORY(Nightbird::Core::Transform),
	NB_FIELD(position),
	NB_FIELD(rotation),
	NB_FIELD(scale)
)

namespace Nightbird::Core
{
	glm::mat4 Transform::GetLocalMatrix() const
	{
		glm::mat4 t = glm::translate(glm::mat4(1.0f), glm::vec3(position));
		glm::mat4 r = glm::mat4_cast(glm::quat(rotation));
		glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3(scale));

		return t * r * s;
	}
}
