#pragma once

#include <volk.h>
#include <vk_mem_alloc.h>

#include <vector>
#include <optional>

namespace Nightbird::Vulkan
{
	class Device
	{
	public:
		Device(VkInstance instance, VkSurfaceKHR surface);
		~Device();

		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags propertyFlags);

		VkCommandBuffer BeginSingleTimeCommands() const;
		void EndSingleTimeCommands(VkCommandBuffer commandBuffer) const;

		VkDevice GetLogical() const;
		VkPhysicalDevice GetPhysical() const;

		VmaAllocator GetAllocator() const;

		VkQueue GetGraphicsQueue() const;
		VkQueue GetPresentQueue() const;

		uint32_t GetGraphicsQueueFamily() const;
		uint32_t GetPresentQueueFamily() const;

		VkCommandBuffer GetCommandBuffer(uint32_t currentFrame) const;

		VkFormat FindSupportedDepthFormat() const;

	private:
		VkDevice m_LogicalDevice;
		VkPhysicalDevice m_PhysicalDevice;

		VkInstance m_Instance;
		VkSurfaceKHR m_Surface;

		VmaAllocator m_Allocator;

		VkCommandPool m_CommandPool;

		uint32_t m_GraphicsQueueFamily;
		uint32_t m_PresentQueueFamily;

		std::vector<VkCommandBuffer> m_CommandBuffers;

		VkQueue m_GraphicsQueue;
		VkQueue m_PresentQueue;

		void SelectPhysicalDevice();
		void CreateLogicalDevice();

		void CreateAllocator();

		void CreateCommandPool();
		void CreateCommandBuffers();
	};
}