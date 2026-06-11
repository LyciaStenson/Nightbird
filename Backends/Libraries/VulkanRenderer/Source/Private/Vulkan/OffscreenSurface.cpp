#include "Vulkan/OffscreenSurface.h"

#include "Vulkan/Device.h"

#include <array>

namespace Nightbird::Vulkan
{
	OffscreenSurface::OffscreenSurface(Device* device, uint32_t width, uint32_t height, VkFormat colorFormat, VkFormat depthFormat)
		: m_Device(device), m_Width(width), m_Height(height), m_ColorFormat(colorFormat), m_DepthFormat(depthFormat)
	{
		Create();
	}

	OffscreenSurface::~OffscreenSurface()
	{
		Destroy();
	}

	void OffscreenSurface::Create()
	{
		m_ColorTexture = std::make_unique<Texture>(
			m_Device, m_Width, m_Height, m_ColorFormat,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_IMAGE_ASPECT_COLOR_BIT);
		
		m_DepthTexture = std::make_unique<Texture>(
			m_Device, m_Width, m_Height, m_DepthFormat,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VK_IMAGE_ASPECT_DEPTH_BIT);

		m_RenderPass = std::make_unique<RenderPass>(
			m_Device, m_ColorFormat, m_DepthFormat,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		std::array<VkImageView, 2> attachments = {
			m_ColorTexture->GetImageView(), m_DepthTexture->GetImageView()
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = m_RenderPass->Get();
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = m_Width;
		framebufferInfo.height = m_Height;
		framebufferInfo.layers = 1;

		vkCreateFramebuffer(m_Device->GetLogical(), &framebufferInfo, nullptr, &m_Framebuffer);
	}

	void OffscreenSurface::Destroy()
	{
		vkDeviceWaitIdle(m_Device->GetLogical());

		if (m_Framebuffer != VK_NULL_HANDLE)
		{
			vkDestroyFramebuffer(m_Device->GetLogical(), m_Framebuffer, nullptr);
			m_Framebuffer = VK_NULL_HANDLE;
		}

		m_RenderPass.reset();
		m_ColorTexture.reset();
		m_DepthTexture.reset();
	}

	Texture& OffscreenSurface::GetColorTexture()
	{
		return *m_ColorTexture;
	}

	Texture& OffscreenSurface::GetDepthTexture()
	{
		return *m_DepthTexture;
	}
	
	VkExtent2D OffscreenSurface::GetExtent() const
	{
		return { m_Width, m_Height };
	}
	
	VkFramebuffer OffscreenSurface::GetFramebuffer() const
	{
		return m_Framebuffer;
	}

	RenderPass& OffscreenSurface::GetRenderPass() const
	{
		return *m_RenderPass;
	}

	bool OffscreenSurface::NeedsResize() const
	{
		return false;
	}

	RenderSurfaceType OffscreenSurface::GetSurfaceType() const
	{
		return RenderSurfaceType::Offscreen;
	}

	uint32_t OffscreenSurface::GetWidth() const
	{
		return m_Width;
	}

	uint32_t OffscreenSurface::GetHeight() const
	{
		return m_Height;
	}

	void OffscreenSurface::Resize(uint32_t width, uint32_t height)
	{
		m_Width = width;
		m_Height = height;

		Destroy();
		Create();
	}
}
