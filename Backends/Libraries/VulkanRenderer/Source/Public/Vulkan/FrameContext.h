#pragma once

#include <volk.h>

namespace Nightbird::Vulkan
{
	struct FrameContext
	{
		VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
		uint32_t frameIndex = 0;
	};
}
