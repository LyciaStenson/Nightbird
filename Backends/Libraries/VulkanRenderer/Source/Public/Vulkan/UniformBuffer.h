#pragma once

#include "Vulkan/Buffer.h"

#include <volk.h>

#include <memory>

namespace Nightbird::Vulkan
{
	class Device;

	class UniformBuffer
	{
	public:
		UniformBuffer(Device* device, VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags);
		~UniformBuffer();

		UniformBuffer(const UniformBuffer&) = delete;
		UniformBuffer& operator=(const UniformBuffer&) = delete;
		UniformBuffer(UniformBuffer&&) = default;
		UniformBuffer& operator=(UniformBuffer&&) = default;

		VkBuffer Get() const;
		void* GetMappedData() const;

	private:
		std::unique_ptr<Buffer> m_Buffer;

		Device* m_Device;

		void* m_MappedData = nullptr;
	};
}