#include "Vulkan/SwapChain.h"

#include "Vulkan/Image.h"
#include "Vulkan/Device.h"
#include "Vulkan/Sync.h"
#include "Vulkan/ImageUtils.h"
#include "Vulkan/SwapChainUtils.h"

#include <iostream>
#include <algorithm>
#include <vector>
#include <array>
#include <limits>

namespace Nightbird::Vulkan
{
	SwapChain::SwapChain(Device* device, Sync* sync, VkSurfaceKHR surface, uint32_t width, uint32_t height)
		: m_Device(device), m_Sync(sync), m_Surface(surface)
	{
		CreateSwapChain(width, height);
		CreateDepthResources(width, height);
	}

	SwapChain::~SwapChain()
	{
		CleanupSwapChain();
	}

	VkSwapchainKHR SwapChain::Get() const
	{
		return m_SwapChain;
	}

	uint32_t SwapChain::GetImageCount() const
	{
		return m_ImageCount;
	}

	uint32_t SwapChain::GetMinImageCount() const
	{
		return m_MinImageCount;
	}

	const std::vector<std::unique_ptr<Image>>& SwapChain::GetImages() const
	{
		return m_Images;
	}

	const Image& SwapChain::GetDepthImage() const
	{
		return *m_DepthImage;
	}

	VkFormat SwapChain::GetColorFormat() const
	{
		return m_ColorFormat;
	}

	VkFormat SwapChain::GetDepthFormat() const
	{
		return m_DepthFormat;
	}

	void SwapChain::CreateSwapChain(uint32_t width, uint32_t height)
	{
		VkDevice logicalDevice = m_Device->GetLogical();
		VkPhysicalDevice physicalDevice = m_Device->GetPhysical();

		SwapChainSupportDetails supportDetails = QuerySwapChainSupport(physicalDevice, m_Surface);

		m_SurfaceFormat = ChooseSwapSurfaceFormat(supportDetails.formats);
		m_PresentMode = ChooseSwapPresentMode(supportDetails.presentModes);

		m_Extent = ChooseSwapExtent(supportDetails.capabilities, width, height);

		m_MinImageCount = supportDetails.capabilities.minImageCount;
		m_ImageCount = m_MinImageCount + 1;
		if (supportDetails.capabilities.maxImageCount > 0 && m_ImageCount > supportDetails.capabilities.maxImageCount)
			m_ImageCount = supportDetails.capabilities.maxImageCount;

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_Surface;

		createInfo.minImageCount = m_ImageCount;
		createInfo.imageFormat = m_SurfaceFormat.format;
		createInfo.imageColorSpace = m_SurfaceFormat.colorSpace;
		createInfo.imageExtent = m_Extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		uint32_t graphicsFamily = m_Device->GetGraphicsQueueFamily();
		uint32_t presentFamily = m_Device->GetPresentQueueFamily();
		uint32_t queueFamilyIndices[] = { graphicsFamily, presentFamily };
		
		if (graphicsFamily != presentFamily)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}

		createInfo.preTransform = supportDetails.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = m_PresentMode;
		createInfo.clipped = VK_TRUE;

		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS)
		{
			std::cerr << "Failed to create swap chain" << std::endl;
			return;
		}

		vkGetSwapchainImagesKHR(logicalDevice, m_SwapChain, &m_ImageCount, nullptr);

		std::vector<VkImage> rawImages(m_ImageCount);
		vkGetSwapchainImagesKHR(logicalDevice, m_SwapChain, &m_ImageCount, rawImages.data());

		m_Images.clear();
		for (VkImage image : rawImages)
		{
			m_Images.push_back(std::make_unique<Image>(m_Device, image, m_SurfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT));
		}

		m_ColorFormat = m_SurfaceFormat.format;
	}

	void SwapChain::CreateDepthResources(uint32_t width, uint32_t height)
	{
		m_DepthFormat = FindDepthFormat(m_Device->GetPhysical());

		ImageConfig config;
		config.width = m_Extent.width;
		config.height = m_Extent.height;
		config.format = m_DepthFormat;
		config.usageFlags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		config.aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;
		m_DepthImage = std::make_unique<Image>(m_Device, config);
		
		m_DepthImage->TransitionImageLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	}

	void SwapChain::CleanupSwapChain()
	{
		vkDestroySwapchainKHR(m_Device->GetLogical(), m_SwapChain, nullptr);
		m_SwapChain = VK_NULL_HANDLE;

		m_Images.clear();
	}
	
	void SwapChain::Recreate(VkRenderPass renderPass, uint32_t width, uint32_t height)
	{
		if (width == m_Extent.width && height == m_Extent.height)
			return;
		
		vkDeviceWaitIdle(m_Device->GetLogical());

		CleanupSwapChain();
		m_Sync->CleanupSyncObjects();

		CreateSwapChain(width, height);
		CreateDepthResources(width, height);
	}

	VkSurfaceFormatKHR SwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& availableFormat : availableFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return availableFormat;
			}
		}
		return availableFormats[0];
	}

	VkPresentModeKHR SwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		for (const auto& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return availablePresentMode;
			}
		}
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D SwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			return capabilities.currentExtent;
		
		VkExtent2D actualExtent = { width, height };

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}
