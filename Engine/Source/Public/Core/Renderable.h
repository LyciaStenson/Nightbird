#pragma once

#include <glm/glm.hpp>

namespace Nightbird::Core
{
	class MeshPrimitive;

	struct Renderable
	{
		const MeshPrimitive* primitive;
		glm::mat4 transform;
	};
}