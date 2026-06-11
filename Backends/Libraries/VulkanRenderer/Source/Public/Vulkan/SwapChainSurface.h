#pragma once

#include "Vulkan/RenderSurface.h"

#include <volk.h>

#include <memory>
#include <vector>

namespace Nightbird::Core
{
	class Platform;
}

namespace Nightbird::Vulkan
{
	class Device;
	class Sync;
	class SwapChain;
	class RenderPass;

	class SwapChainSurface : public RenderSurface
	{
	public:
		SwapChainSurface(Core::Platform* platform, Device& device, Sync& sync, SwapChain& swapChain);
		~SwapChainSurface();

		VkExtent2D GetExtent() const override;
		RenderPass& GetRenderPass() const override;
		bool NeedsResize() const override;
		RenderSurfaceType GetSurfaceType() const override;

		VkFramebuffer AcquireFramebuffer(VkSemaphore imageAvailableSemaphore);
		void Present(VkSemaphore renderFinishedSemaphore);
		
		uint32_t GetWidth() const override;
		uint32_t GetHeight() const override;

		void Resize(uint32_t width, uint32_t height) override;

	private:
		Device& m_Device;
		Sync& m_Sync;
		SwapChain& m_SwapChain;
		
		std::unique_ptr<RenderPass> m_RenderPass;
		
		Core::Platform* m_Platform = nullptr;

		std::vector<VkFramebuffer> m_Framebuffers;
		uint32_t m_CurrentImageIndex = 0;

		bool m_NeedsResize = false;

		void CreateRenderPass();
		void CreateFramebuffers();
	};
}
