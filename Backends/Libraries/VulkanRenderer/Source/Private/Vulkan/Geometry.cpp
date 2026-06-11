#include "Vulkan/Geometry.h"

#include "Vulkan/Device.h"
#include "Vulkan/BufferUtils.h"

#include "Core/MeshPrimitive.h"

#include <cstring>

namespace Nightbird::Vulkan
{
	Geometry::Geometry(Device* device, const void* vertexData, VkDeviceSize vertexSize, const void* indexData, VkDeviceSize indexSide, uint32_t indexCount)
		: m_IndexCount(indexCount)
	{
		CreateVertexBuffer(device, vertexData, vertexSize);
		CreateIndexBuffer(device, indexData, indexSide);
	}

	void Geometry::CreateVertexBuffer(Device* device, const void* data, VkDeviceSize size)
	{
		Buffer stagingBuffer(device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		void* mapped = stagingBuffer.Map();
		memcpy(mapped, data, (size_t)size);
		stagingBuffer.Unmap();

		m_VertexBuffer = std::make_unique<Buffer>(device, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		CopyBuffer(device, stagingBuffer.Get(), m_VertexBuffer->Get(), size);
	}

	void Geometry::CreateIndexBuffer(Device* device, const void* data, VkDeviceSize size)
	{
		Buffer stagingBuffer(device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		void* mapped = stagingBuffer.Map();
		memcpy(mapped, data, (size_t)size);
		stagingBuffer.Unmap();

		m_IndexBuffer = std::make_unique<Buffer>(device, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		CopyBuffer(device, stagingBuffer.Get(), m_IndexBuffer->Get(), size);
	}

	VkBuffer Geometry::GetVertexBuffer() const
	{
		return m_VertexBuffer->Get();
	}

	VkBuffer Geometry::GetIndexBuffer() const
	{
		return m_IndexBuffer->Get();
	}

	uint32_t Geometry::GetIndexCount() const
	{
		return m_IndexCount;
	}
}
