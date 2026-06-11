#pragma once

#include <volk.h>

namespace Nightbird::Vulkan
{
	class Device;

	struct ImageConfig
	{
		uint32_t width = 0;
		uint32_t height = 0;
		VkFormat format = VK_FORMAT_UNDEFINED;
		VkImageUsageFlags usageFlags = 0;
		VkMemoryPropertyFlags memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
		uint32_t arrayLayers = 1;
		VkImageCreateFlags flags = 0;
		VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
	};

	class Image
	{
	public:
		Image(Device* device, const ImageConfig& config);
		Image(Device* device, VkImage existingImage, VkFormat format, VkImageAspectFlags aspectFlags);
		~Image();

		VkImage Get() const;
		VkImageView GetImageView() const;
		
		void TransitionImageLayout(VkImageLayout newLayout);
		void TransitionImageLayout(VkCommandBuffer commandBuffer, VkImageLayout newLayout);

		void SetLayout(VkImageLayout layout);

	private:
		VkImage m_Image;
		VkImageLayout m_CurrentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		VkFormat m_Format;

		VkImageView m_ImageView;
		VkDeviceMemory m_Memory;

		Device* m_Device;

		bool m_OwnsImage;

		uint32_t m_LayerCount = 1;

		void CreateImage(const ImageConfig& config);
		void CreateImageView(const ImageConfig& config);
		void CreateImageView(VkImageAspectFlags aspectFlags);
	};
}