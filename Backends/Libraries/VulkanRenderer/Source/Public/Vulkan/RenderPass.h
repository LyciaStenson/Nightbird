#pragma once

#include <volk.h>

namespace Nightbird::Vulkan
{
	class Device;

	class RenderPass
	{
	public:
		RenderPass(Device* device, VkFormat colorFormat, VkFormat depthFormat, VkImageLayout finalColorLayout);
		~RenderPass();

		VkRenderPass Get() const;

		void Begin(VkCommandBuffer commandBuffer, VkFramebuffer framebuffer, VkExtent2D extent);
		void End(VkCommandBuffer commandBuffer);

		void BeginCommandBuffer(VkCommandBuffer commandBuffer);
		void EndCommandBuffer(VkCommandBuffer commandBuffer);

	private:
		VkRenderPass m_RenderPass;

		Device* m_Device;

		void Create(VkFormat colorFormat, VkFormat depthFormat, VkImageLayout finalColorLayout);
	};
}