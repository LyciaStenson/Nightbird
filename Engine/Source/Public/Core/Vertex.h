#pragma once

#include <array>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Nightbird::Core
{
	struct VertexPos
	{
		glm::vec3 position;
	};

	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 baseColorTexCoord;
		glm::vec2 metallicRoughnessTexCoord;
		glm::vec2 normalTexCoord;
	};
}