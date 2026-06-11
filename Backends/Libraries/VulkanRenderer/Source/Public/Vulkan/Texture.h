#pragma once

#include "Vulkan/Image.h"

#include <memory>

namespace Nightbird::Core
{
	class Texture;
	class Cubemap;
}

namespace Nightbird::Vulkan
{
	class Device;

	class Texture
	{
	public:
		// Create from CPU texture data
		Texture(Device* device, const Core::Texture& texture, bool sRGB = true);
		// Create from CPU cubemap data
		Texture(Device* device, const Core::Cubemap& cubemap, bool sRGB = false);
		// Create for render target
		Texture(Device* device, uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usageFlags, VkImageAspectFlags aspectFlags);

		static Texture CreateDefaultCubemap(Device* device);

		Texture(Texture&& other) noexcept;
		Texture& operator=(Texture&& other) noexcept;
		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;

		~Texture();

		VkImageView GetImageView() const;
		VkSampler GetSampler() const;

		void TransitionToShaderRead(VkCommandBuffer commandBuffer);
		void TransitionToColor(VkCommandBuffer commandBuffer);

		void SetImageLayout(VkImageLayout layout);

	private:
		std::unique_ptr<Image> m_Image;

		VkSampler m_Sampler = VK_NULL_HANDLE;

		Device* m_Device = nullptr;

		void CreateFromTexture(const uint8_t* data, uint32_t width, uint32_t height, bool sRGB);
		void CreateFromCubemap(const uint8_t* data, uint32_t faceSize, bool sRGB);
		void CreateSampler(VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT);
	};
}