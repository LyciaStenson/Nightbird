#include "Vulkan/Image.h"

#include "Vulkan/Device.h"
#include "Vulkan/ImageUtils.h"

#include <iostream>

namespace Nightbird::Vulkan
{
	Image::Image(Device* device, const ImageConfig& config)
		: m_Device(device), m_Format(config.format), m_OwnsImage(true), m_LayerCount(config.arrayLayers)
	{
		CreateImage(config);
		CreateImageView(config);
	}

	Image::Image(Device* device, VkImage existingImage, VkFormat format, VkImageAspectFlags aspectFlags)
		: m_Device(device), m_Format(format), m_Image(existingImage), m_OwnsImage(false)
	{
		CreateImageView(aspectFlags);
	}

	Image::~Image()
	{
		VkDevice logicalDevice = m_Device->GetLogical();

		if (m_ImageView != VK_NULL_HANDLE)
			vkDestroyImageView(logicalDevice, m_ImageView, nullptr);

		if (m_OwnsImage)
		{
			if (m_Image != VK_NULL_HANDLE)
				vkDestroyImage(logicalDevice, m_Image, nullptr);
			if (m_Memory != VK_NULL_HANDLE)
				vkFreeMemory(logicalDevice, m_Memory, nullptr);
		}
	}

	VkImage Image::Get() const
	{
		return m_Image;
	}

	VkImageView Image::GetImageView() const
	{
		return m_ImageView;
	}

	void Image::CreateImage(const ImageConfig& config)
	{
		VkDevice logicalDevice = m_Device->GetLogical();

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = config.width;
		imageInfo.extent.height = config.height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = config.arrayLayers;
		imageInfo.format = config.format;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = config.usageFlags;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.flags = config.flags;

		if (vkCreateImage(logicalDevice, &imageInfo, nullptr, &m_Image) != VK_SUCCESS)
		{
			std::cerr << "Failed to create image" << std::endl;
			return;
		}

		VkMemoryRequirements memoryRequirements;
		vkGetImageMemoryRequirements(logicalDevice, m_Image, &memoryRequirements);

		VkMemoryAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocateInfo.allocationSize = memoryRequirements.size;
		allocateInfo.memoryTypeIndex = m_Device->FindMemoryType(memoryRequirements.memoryTypeBits, config.memoryPropertyFlags);

		if (vkAllocateMemory(logicalDevice, &allocateInfo, nullptr, &m_Memory) != VK_SUCCESS)
		{
			std::cerr << "Failed to allocate image memory" << std::endl;
			return;
		}

		vkBindImageMemory(logicalDevice, m_Image, m_Memory, 0);
	}

	void Image::CreateImageView(const ImageConfig& config)
	{
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = m_Image;
		viewInfo.viewType = config.viewType;
		viewInfo.format = config.format;

		viewInfo.components = {
			VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
			VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY
		};
		
		viewInfo.subresourceRange.aspectMask = config.aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = config.arrayLayers;

		if (vkCreateImageView(m_Device->GetLogical(), &viewInfo, nullptr, &m_ImageView) != VK_SUCCESS)
			std::cerr << "Failed to create image view" << std::endl;
	}

	void Image::CreateImageView(VkImageAspectFlags aspectFlags)
	{
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = m_Image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = m_Format;

		viewInfo.components = {
			VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
			VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY
		};

		viewInfo.subresourceRange.aspectMask = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(m_Device->GetLogical(), &viewInfo, nullptr, &m_ImageView) != VK_SUCCESS)
			std::cerr << "Failed to create image view" << std::endl;
	}

	void Image::TransitionImageLayout(VkImageLayout newLayout)
	{
		VkCommandBuffer commandBuffer = m_Device->BeginSingleTimeCommands();

		VkImageMemoryBarrier memoryBarrier{};
		memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		memoryBarrier.oldLayout = m_CurrentLayout;
		memoryBarrier.newLayout = newLayout;
		memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		memoryBarrier.image = m_Image;
		memoryBarrier.subresourceRange.baseMipLevel = 0;
		memoryBarrier.subresourceRange.levelCount = 1;
		memoryBarrier.subresourceRange.baseArrayLayer = 0;
		memoryBarrier.subresourceRange.layerCount = m_LayerCount;

		memoryBarrier.srcAccessMask = 0;
		memoryBarrier.dstAccessMask = 0;

		VkPipelineStageFlags sourceStageFlags;
		VkPipelineStageFlags destinationStageFlags;

		if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			memoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

			if (HasStencilComponent(m_Format))
			{
				memoryBarrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
		}
		else
		{
			memoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		}

		if (m_CurrentLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			memoryBarrier.srcAccessMask = 0;
			memoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (m_CurrentLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			memoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (m_CurrentLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			memoryBarrier.srcAccessMask = 0;
			memoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			sourceStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStageFlags = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else if (m_CurrentLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		{
			memoryBarrier.srcAccessMask = 0;
			memoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			sourceStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		}
		else if (m_CurrentLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			memoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			memoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			destinationStageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else
		{
			std::cerr << "Unsupported layout transition" << std::endl;
			return;
		}

		m_CurrentLayout = newLayout;

		vkCmdPipelineBarrier
		(
			commandBuffer,
			sourceStageFlags, destinationStageFlags,
			0,
			0, nullptr,
			0, nullptr,
			1, &memoryBarrier
		);

		m_Device->EndSingleTimeCommands(commandBuffer);
	}

	void Image::TransitionImageLayout(VkCommandBuffer commandBuffer, VkImageLayout newLayout)
	{
		VkImageMemoryBarrier memoryBarrier{};
		memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		memoryBarrier.oldLayout = m_CurrentLayout;
		memoryBarrier.newLayout = newLayout;
		memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		memoryBarrier.image = m_Image;
		memoryBarrier.subresourceRange.baseMipLevel = 0;
		memoryBarrier.subresourceRange.levelCount = 1;
		memoryBarrier.subresourceRange.baseArrayLayer = 0;
		memoryBarrier.subresourceRange.layerCount = m_LayerCount;

		memoryBarrier.srcAccessMask = 0;
		memoryBarrier.dstAccessMask = 0;

		VkPipelineStageFlags sourceStageFlags;
		VkPipelineStageFlags destinationStageFlags;

		if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			memoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

			if (HasStencilComponent(m_Format))
			{
				memoryBarrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
		}
		else
		{
			memoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		}

		if (m_CurrentLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			memoryBarrier.srcAccessMask = 0;
			memoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (m_CurrentLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			memoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (m_CurrentLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			memoryBarrier.srcAccessMask = 0;
			memoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			sourceStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStageFlags = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else if (m_CurrentLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		{
			memoryBarrier.srcAccessMask = 0;
			memoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			sourceStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		}
		else if (m_CurrentLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			memoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			memoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			destinationStageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (m_CurrentLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		{
			memoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			memoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			sourceStageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			destinationStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		}
		else
		{
			std::cerr << "Unsupported layout transition" << std::endl;
			return;
		}

		m_CurrentLayout = newLayout;

		vkCmdPipelineBarrier
		(
			commandBuffer,
			sourceStageFlags,
			destinationStageFlags,
			0,
			0, nullptr,
			0, nullptr,
			1, &memoryBarrier
		);
	}

	void Image::SetLayout(VkImageLayout layout)
	{
		m_CurrentLayout = layout;
	}
}