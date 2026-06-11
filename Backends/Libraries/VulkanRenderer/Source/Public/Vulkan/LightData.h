#pragma once

#include <glm/glm.hpp>

namespace Nightbird::Vulkan
{
	constexpr int MAX_DIRECTIONAL_LIGHTS = 16;
	constexpr int MAX_POINT_LIGHTS = 16;

	struct alignas(16) DirectionalLightData
	{
		alignas(16) glm::vec4 direction;
		alignas(16) glm::vec4 colorIntensity;
	};

	struct alignas(16) DirectionalLightMetaUBO
	{
		uint32_t count;
	};

	struct alignas(16) PointLightData
	{
		alignas(16) glm::vec4 positionRadius;
		alignas(16) glm::vec4 colorIntensity;
	};

	struct alignas(16) PointLightMetaUBO
	{
		uint32_t count;
	};
}