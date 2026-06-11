#include "Vulkan/UniformBuffer.h"

#include "Vulkan/Buffer.h"
#include "Vulkan/Device.h"

#include <iostream>

namespace Nightbird::Vulkan
{
	UniformBuffer::UniformBuffer(Device* device, VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags)
		: m_Device(device)
	{
		m_Buffer = std::make_unique<Buffer>(device, size, usageFlags, propertyFlags);
		m_MappedData = m_Buffer->Map();
	}

	UniformBuffer::~UniformBuffer()
	{
		if (m_MappedData)
			vmaUnmapMemory(m_Device->GetAllocator(), m_Buffer->GetAllocation());
	}

	VkBuffer UniformBuffer::Get() const
	{
		return m_Buffer->Get();
	}

	void* UniformBuffer::GetMappedData() const
	{
		return m_MappedData;
	}
}