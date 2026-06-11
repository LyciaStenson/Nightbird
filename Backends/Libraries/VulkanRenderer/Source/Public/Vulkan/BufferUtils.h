#pragma once

#include "volk.h"

namespace Nightbird::Vulkan
{
	class Device;

	void CopyBuffer(Device* device, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	void CopyBufferToImage(Device* device, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
}