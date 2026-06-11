#pragma once

#include "Vulkan/RenderSurface.h"
#include "Vulkan/Texture.h"
#include "Vulkan/RenderPass.h"

#include <memory>

namespace Nightbird::Vulkan
{
	class Device;

	class OffscreenSurface : public RenderSurface
	{
	public:
		OffscreenSurface(Device* device, uint32_t width, uint32_t height, VkFormat colorFormat, VkFormat depthFormat);
		~OffscreenSurface();

		Texture& GetColorTexture();
		Texture& GetDepthTexture();

		VkFramebuffer GetFramebuffer() const;

		VkExtent2D GetExtent() const override;
		RenderPass& GetRenderPass() const override;
		bool NeedsResize() const override;
		RenderSurfaceType GetSurfaceType() const override;
		
		uint32_t GetWidth() const override;
		uint32_t GetHeight() const override;

		void Resize(uint32_t width, uint32_t height) override;

		VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;

	private:
		Device* m_Device;

		uint32_t m_Width;
		uint32_t m_Height;

		VkFormat m_ColorFormat;
		VkFormat m_DepthFormat;

		std::unique_ptr<Texture> m_ColorTexture;
		std::unique_ptr<Texture> m_DepthTexture;
		std::unique_ptr<RenderPass> m_RenderPass;
		
		VkFramebuffer m_Framebuffer = VK_NULL_HANDLE;
		
		void Create();
		void Destroy();
	};
}
