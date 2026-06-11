#pragma once

#include <volk.h>

#include <vector>

namespace Nightbird::Vulkan
{
	class Device;

	class EnvironmentDescriptorSetManager
	{
	public:
		EnvironmentDescriptorSetManager(Device* device, VkDescriptorSetLayout layout, VkDescriptorPool pool);

		const std::vector<VkDescriptorSet>& GetDescriptorSets() const;

		void UpdateSkybox(uint32_t frameIndex, VkImageView imageView, VkSampler sampler);

	private:
		Device* m_Device;
		std::vector<VkDescriptorSet> m_DescriptorSets;

		void CreateDescriptorSets(VkDescriptorSetLayout layout, VkDescriptorPool pool);
	};
}
