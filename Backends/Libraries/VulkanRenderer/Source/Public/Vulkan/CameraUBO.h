#pragma once

#include <glm/glm.hpp>

namespace Nightbird::Vulkan
{
	struct alignas(16) CameraUBO
	{
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 projection;
		alignas(16) glm::vec4 position;
	};
}