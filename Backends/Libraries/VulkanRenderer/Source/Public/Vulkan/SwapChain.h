#pragma once

#include <volk.h>

#include <memory>
#include <vector>

namespace Nightbird::Vulkan
{
	class Device;
	class Sync;
	class Image;

	class SwapChain
	{
	public:
		SwapChain(Device* device, Sync* sync, VkSurfaceKHR surface, uint32_t width, uint32_t height);
		~SwapChain();

		VkSwapchainKHR Get() const;

		uint32_t GetImageCount() const;
		uint32_t GetMinImageCount() const;

		const std::vector<std::unique_ptr<Image>>& GetImages() const;
		const Image& GetDepthImage() const;

		VkFormat GetColorFormat() const;
		VkFormat GetDepthFormat() const;
		
		void Recreate(VkRenderPass renderPass, uint32_t width, uint32_t height);
		
		VkExtent2D m_Extent;

		VkSurfaceFormatKHR m_SurfaceFormat;
		VkPresentModeKHR m_PresentMode;

	private:
		void CreateSwapChain(uint32_t width, uint32_t height);
		void CreateDepthResources(uint32_t width, uint32_t height);

		void CleanupSwapChain();

		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height);

		VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;

		VkFormat m_ColorFormat;
		VkFormat m_DepthFormat;

		std::vector<std::unique_ptr<Image>> m_Images;
		std::unique_ptr<Image> m_DepthImage;
		
		Device* m_Device;
		Sync* m_Sync;
		
		uint32_t m_ImageCount;
		uint32_t m_MinImageCount;
	};
}
