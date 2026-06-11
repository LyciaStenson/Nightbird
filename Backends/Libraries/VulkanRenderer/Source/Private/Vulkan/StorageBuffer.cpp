#include "Vulkan/StorageBuffer.h"

#include "Vulkan/Buffer.h"
#include "Vulkan/Device.h"

#include <cstring>
#include <iostream>

namespace Nightbird::Vulkan
{
	StorageBuffer::StorageBuffer(Device* device, VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags)
		: m_Device(device)
	{
		m_Buffer = std::make_unique<Buffer>(device, size, usageFlags | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, propertyFlags);

		if (propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
			m_MappedData = m_Buffer->Map();
	}

	StorageBuffer::~StorageBuffer()
	{
		if (m_MappedData)
			vmaUnmapMemory(m_Device->GetAllocator(), m_Buffer->GetAllocation());
	}

	VkBuffer StorageBuffer::Get() const
	{
		return m_Buffer->Get();
	}

	void* StorageBuffer::GetMappedData() const
	{
		return m_MappedData;
	}

	void StorageBuffer::UploadData(const void* data, VkDeviceSize size)
	{
		if (!m_MappedData)
			return;

		std::memcpy(m_MappedData, data, static_cast<size_t>(size));
	}
}
