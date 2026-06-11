#include "Vulkan/Buffer.h"

#include "Vulkan/Device.h"

#include <iostream>

namespace Nightbird::Vulkan
{
	Buffer::Buffer(Device* device, VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags)
		: m_Device(device), m_Size(size)
	{
		CreateBuffer(usageFlags, propertyFlags);
	}

	Buffer::~Buffer()
	{
		if (m_Buffer != VK_NULL_HANDLE)
		{
			VmaAllocator allocator = m_Device->GetAllocator();
			vmaDestroyBuffer(allocator, m_Buffer, m_Allocation);
		}
	}

	VkBuffer Buffer::Get() const
	{
		return m_Buffer;
	}

	VmaAllocation Buffer::GetAllocation() const
	{
		return m_Allocation;
	}

	void* Buffer::Map()
	{
		void* mappedData;

		if (vmaMapMemory(m_Device->GetAllocator(), m_Allocation, &mappedData) != VK_SUCCESS)
		{
			std::cerr << "Failed to map Buffer memory" << std::endl;
		}

		return mappedData;
	}

	void Buffer::Unmap()
	{
		vmaUnmapMemory(m_Device->GetAllocator(), m_Allocation);
	}

	void Buffer::CreateBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags)
	{
		VmaAllocator allocator = m_Device->GetAllocator();

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = m_Size;
		bufferInfo.usage = usageFlags;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocationInfo{};
		allocationInfo.usage = VMA_MEMORY_USAGE_AUTO;
		allocationInfo.flags = 0;

		if (propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
		{
			allocationInfo.flags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

			if (!(propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
			{
				allocationInfo.flags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;
			}
		}

		if (vmaCreateBuffer(allocator, &bufferInfo, &allocationInfo, &m_Buffer, &m_Allocation, nullptr) != VK_SUCCESS)
		{
			std::cerr << "Failed to create buffer" << std::endl;
		}
	}
}