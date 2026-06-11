#include "Vulkan/SwapChainSurface.h"

#include "Vulkan/Image.h"
#include "Vulkan/Device.h"
#include "Vulkan/Sync.h"
#include "Vulkan/SwapChain.h"
#include "Vulkan/RenderPass.h"

#include "Core/Platform.h"
#include "Core/Log.h"

#include <volk.h>

namespace Nightbird::Vulkan
{
	SwapChainSurface::SwapChainSurface(Core::Platform* platform, Device& device, Sync& sync, SwapChain& swapChain)
		: m_Platform(platform), m_Device(device), m_Sync(sync), m_SwapChain(swapChain)
	{
		CreateRenderPass();
		CreateFramebuffers();
	}

	SwapChainSurface::~SwapChainSurface()
	{
		for (auto framebuffer : m_Framebuffers)
		{
			vkDestroyFramebuffer(m_Device.GetLogical(), framebuffer, nullptr);
		}
	}

	VkExtent2D SwapChainSurface::GetExtent() const
	{
		return m_SwapChain.m_Extent;
	}

	VkFramebuffer SwapChainSurface::AcquireFramebuffer(VkSemaphore imageAvailableSemaphore)
	{
		VkResult result = vkAcquireNextImageKHR(
			m_Device.GetLogical(), m_SwapChain.Get(),
			UINT64_MAX,
			imageAvailableSemaphore,
			VK_NULL_HANDLE,
			&m_CurrentImageIndex
		);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			m_NeedsResize = true;
			return VK_NULL_HANDLE;
		}
		else if (result != VK_SUCCESS)
		{
			Core::Log::Error("Failed to acquire swap chain image");
			return VK_NULL_HANDLE;
		}

		return m_Framebuffers[m_CurrentImageIndex];
	}

	RenderPass& SwapChainSurface::GetRenderPass() const
	{
		return *m_RenderPass;
	}

	bool SwapChainSurface::NeedsResize() const
	{
		return m_NeedsResize;
	}
	
	RenderSurfaceType SwapChainSurface::GetSurfaceType() const
	{
		return RenderSurfaceType::SwapChain;
	}
	
	void SwapChainSurface::Present(VkSemaphore renderFinishedSemaphore)
	{
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &renderFinishedSemaphore;

		VkSwapchainKHR swapChains[] = { m_SwapChain.Get() };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &m_CurrentImageIndex;

		VkResult result = vkQueuePresentKHR(m_Device.GetPresentQueue(), &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			m_NeedsResize = true;
			return;
		}
		else if (result != VK_SUCCESS)
		{
			Core::Log::Error("Failed to present swap chain image");
		}
	}

	uint32_t SwapChainSurface::GetWidth() const
	{
		return m_SwapChain.m_Extent.width;
	}

	uint32_t SwapChainSurface::GetHeight() const
	{
		return m_SwapChain.m_Extent.height;
	}

	void SwapChainSurface::Resize(uint32_t width, uint32_t height)
	{
		if (width == m_SwapChain.m_Extent.width && height == m_SwapChain.m_Extent.height)
			return;

		m_SwapChain.Recreate(m_RenderPass->Get(), width, height);
		CreateFramebuffers();
		m_Sync.CreateSyncObjects();

		m_NeedsResize = false;
	}

	void SwapChainSurface::CreateRenderPass()
	{
		m_RenderPass = std::make_unique<RenderPass>(&m_Device, m_SwapChain.GetColorFormat(), m_SwapChain.GetDepthFormat(), VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
	}

	void SwapChainSurface::CreateFramebuffers()
	{
		vkDeviceWaitIdle(m_Device.GetLogical());

		for (auto framebuffer : m_Framebuffers)
		{
			vkDestroyFramebuffer(m_Device.GetLogical(), framebuffer, nullptr);
		}
		m_Framebuffers.clear();

		const auto& images = m_SwapChain.GetImages();
		m_Framebuffers.reserve(images.size());

		for (const auto& image : images)
		{
			VkImageView attachments[2] = {};
			attachments[0] = image->GetImageView();
			attachments[1] = m_SwapChain.GetDepthImage().GetImageView();
			
			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = m_RenderPass->Get();
			framebufferInfo.attachmentCount = 2;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = m_SwapChain.m_Extent.width;
			framebufferInfo.height = m_SwapChain.m_Extent.height;
			framebufferInfo.layers = 1;

			VkFramebuffer framebuffer;
			if (vkCreateFramebuffer(m_Device.GetLogical(), &framebufferInfo, nullptr, &framebuffer) != VK_SUCCESS)
			{
				Core::Log::Error("Failed to create framebuffer");
				return;
			}

			m_Framebuffers.push_back(framebuffer);
		}
	}
}
