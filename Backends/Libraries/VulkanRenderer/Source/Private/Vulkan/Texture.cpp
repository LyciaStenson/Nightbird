#include "Vulkan/Texture.h"

#include "Vulkan/Device.h"
#include "Vulkan/Buffer.h"
#include "Vulkan/BufferUtils.h"

#include "Core/Texture.h"
#include "Core/Cubemap.h"
#include "Core/Log.h"

#include <array>
#include <cstring>

namespace Nightbird::Vulkan
{
	Texture::Texture(Device* device, const Core::Texture& texture, bool sRGB)
		: m_Device(device)
	{
		switch (texture.GetFormat())
		{
		case Core::TextureFormat::RGBA8:
			CreateFromTexture(texture.GetData().data(), texture.GetWidth(), texture.GetHeight(), sRGB);
			break;
		default:
			Core::Log::Error("Unsupported Vulkan texture format");
			break;
		}

		CreateSampler();
	}

	Texture::Texture(Device* device, const Core::Cubemap& cubemap, bool sRGB)
		: m_Device(device)
	{
		if (!cubemap.HasData())
		{
			Core::Log::Error("Vulkan::Texture: Core::Cubemap has no data");
			return;
		}

		CreateFromCubemap(cubemap.GetData().data(), cubemap.GetFaceSize(), sRGB);
		CreateSampler(VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
	}

	Texture::Texture(Device* device, uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usageFlags, VkImageAspectFlags aspectFlags)
		: m_Device(device)
	{
		ImageConfig config;
		config.width = width;
		config.height = height;
		config.format = format;
		config.usageFlags = usageFlags;
		config.aspectFlags = aspectFlags;
		m_Image = std::make_unique<Image>(device, config);

		if (usageFlags & VK_IMAGE_USAGE_SAMPLED_BIT)
			CreateSampler();
	}

	Texture Texture::CreateDefaultCubemap(Device* device)
	{
		std::vector<uint8_t> data(1 * 1 * 4 * 6, 0);
		for (int i = 0; i < 6; ++i)
			data[i * 4 + 3] = 255;

		Core::Cubemap defaultCubemap(1, std::move(data));
		return Texture(device, defaultCubemap);
	}

	Texture::Texture(Texture&& other) noexcept
		: m_Device(other.m_Device), m_Image(std::move(other.m_Image)), m_Sampler(other.m_Sampler)
	{
		other.m_Device = nullptr;
		other.m_Image = nullptr;
		other.m_Sampler = VK_NULL_HANDLE;
	}

	Texture& Texture::operator=(Texture&& other) noexcept
	{
		if (this != &other)
		{
			m_Device = other.m_Device;
			m_Image = std::move(other.m_Image);
			m_Sampler = other.m_Sampler;

			other.m_Device = nullptr;
			other.m_Image = nullptr;
			other.m_Sampler = VK_NULL_HANDLE;
		}
		return *this;
	}

	Texture::~Texture()
	{
		if (m_Sampler != VK_NULL_HANDLE)
			vkDestroySampler(m_Device->GetLogical(), m_Sampler, nullptr);
	}

	VkImageView Texture::GetImageView() const
	{
		return m_Image->GetImageView();
	}

	VkSampler Texture::GetSampler() const
	{
		return m_Sampler;
	}

	void Texture::TransitionToShaderRead(VkCommandBuffer commandBuffer)
	{
		m_Image->TransitionImageLayout(commandBuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

	void Texture::TransitionToColor(VkCommandBuffer commandBuffer)
	{
		m_Image->TransitionImageLayout(commandBuffer, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	}

	void Texture::SetImageLayout(VkImageLayout layout)
	{
		m_Image->SetLayout(layout);
	}

	void Texture::CreateFromTexture(const uint8_t* data, uint32_t width, uint32_t height, bool sRGB)
	{
		VkDeviceSize imageSize = width * height * 4;

		Buffer stagingBuffer(m_Device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		void* mapped = stagingBuffer.Map();
		memcpy(mapped, data, static_cast<size_t>(imageSize));
		stagingBuffer.Unmap();
		
		ImageConfig config;
		config.width = width;
		config.height = height;
		config.format = sRGB ? VK_FORMAT_R8G8B8A8_SRGB : VK_FORMAT_R8G8B8A8_UNORM;
		config.usageFlags = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		config.aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
		m_Image = std::make_unique<Image>(m_Device, config);

		m_Image->TransitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		CopyBufferToImage(m_Device, stagingBuffer.Get(), m_Image->Get(), width, height);
		m_Image->TransitionImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

	void Texture::CreateFromCubemap(const uint8_t* data, uint32_t faceSize, bool sRGB)
	{
		VkDeviceSize faceBytes = faceSize * faceSize * 4;
		VkDeviceSize totalBytes = faceBytes * 6;

		Buffer stagingBuffer(m_Device, totalBytes, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		void* mapped = stagingBuffer.Map();
		memcpy(mapped, data, static_cast<size_t>(totalBytes));
		stagingBuffer.Unmap();

		ImageConfig config;
		config.width = faceSize;
		config.height = faceSize;
		config.format = sRGB ? VK_FORMAT_R8G8B8A8_SRGB : VK_FORMAT_R8G8B8A8_UNORM;
		config.usageFlags = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		config.aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
		config.arrayLayers = 6;
		config.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
		config.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
		m_Image = std::make_unique<Image>(m_Device, config);

		m_Image->TransitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		std::array<VkBufferImageCopy, 6> regions{};
		for (uint32_t i = 0; i < 6; ++i)
		{
			regions[i].bufferOffset = faceBytes * i;
			regions[i].bufferRowLength = 0;
			regions[i].bufferImageHeight = 0;
			regions[i].imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			regions[i].imageSubresource.mipLevel = 0;
			regions[i].imageSubresource.baseArrayLayer = i;
			regions[i].imageSubresource.layerCount = 1;
			regions[i].imageOffset = { 0, 0, 0 };
			regions[i].imageExtent = { faceSize, faceSize, 1 };
		}

		VkCommandBuffer commandBuffer = m_Device->BeginSingleTimeCommands();
		vkCmdCopyBufferToImage(commandBuffer, stagingBuffer.Get(), m_Image->Get(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 6, regions.data());
		m_Device->EndSingleTimeCommands(commandBuffer);

		m_Image->TransitionImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

	void Texture::CreateSampler(VkSamplerAddressMode adressMode)
	{
		VkPhysicalDeviceProperties deviceProperties{};
		vkGetPhysicalDeviceProperties(m_Device->GetPhysical(), &deviceProperties);

		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = adressMode;
		samplerInfo.addressModeV = adressMode;
		samplerInfo.addressModeW = adressMode;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = deviceProperties.limits.maxSamplerAnisotropy;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		if (vkCreateSampler(m_Device->GetLogical(), &samplerInfo, nullptr, &m_Sampler) != VK_SUCCESS)
			Core::Log::Error("Failed to create texture sampler");
	}
}
