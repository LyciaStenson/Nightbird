#pragma once

#include <volk.h>
#include <vk_mem_alloc.h>

namespace Nightbird::Vulkan
{
	class Device;

	class Buffer
	{
	public:
		Buffer(Device* device, VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags);
		~Buffer();

		VkBuffer Get() const;
		VmaAllocation GetAllocation() const;

		void* Map();
		void Unmap();

	private:
		VkBuffer m_Buffer = VK_NULL_HANDLE;
		VkDeviceSize m_Size = 0;

		VmaAllocation m_Allocation = VK_NULL_HANDLE;

		Device* m_Device;

		void CreateBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags);
	};
}