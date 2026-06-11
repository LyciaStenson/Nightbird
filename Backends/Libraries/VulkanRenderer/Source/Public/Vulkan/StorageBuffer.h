#pragma once

#include <volk.h>

#include <memory>

namespace Nightbird::Vulkan
{
	class Buffer;
	class Device;

	class StorageBuffer
	{
	public:
		StorageBuffer(Device* device, VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags);
		~StorageBuffer();

		StorageBuffer(const StorageBuffer&) = delete;
		StorageBuffer& operator=(const StorageBuffer&) = delete;
		StorageBuffer(StorageBuffer&&) = default;
		StorageBuffer& operator=(StorageBuffer&&) = default;

		VkBuffer Get() const;
		void* GetMappedData() const;

		void UploadData(const void* data, VkDeviceSize size);

	private:
		std::unique_ptr<Buffer> m_Buffer;

		Device* m_Device;

		void* m_MappedData = nullptr;
	};
}