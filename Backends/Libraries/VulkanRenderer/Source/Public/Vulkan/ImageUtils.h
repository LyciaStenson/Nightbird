#pragma once

#include <volk.h>

#include <vector>

namespace Nightbird::Vulkan
{
	VkFormat FindSupportedFormat(VkPhysicalDevice device, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags featureFlags);
	VkFormat FindDepthFormat(VkPhysicalDevice device);

	bool HasStencilComponent(VkFormat format);
}