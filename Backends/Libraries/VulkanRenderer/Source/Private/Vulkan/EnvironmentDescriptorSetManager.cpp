#include "Vulkan/EnvironmentDescriptorSetManager.h"

#include "Vulkan/Config.h"
#include "Vulkan/Device.h"

#include "Core/Log.h"

namespace Nightbird::Vulkan
{
	EnvironmentDescriptorSetManager::EnvironmentDescriptorSetManager(Device* device, VkDescriptorSetLayout layout, VkDescriptorPool pool)
		: m_Device(device)
	{
		CreateDescriptorSets(layout, pool);
	}

	const std::vector<VkDescriptorSet>& EnvironmentDescriptorSetManager::GetDescriptorSets() const
	{
		return m_DescriptorSets;
	}

	void EnvironmentDescriptorSetManager::UpdateSkybox(uint32_t frameIndex, VkImageView imageView, VkSampler sampler)
	{
		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = imageView;
		imageInfo.sampler = sampler;

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet = m_DescriptorSets[frameIndex];
		write.dstBinding = 0;
		write.dstArrayElement = 0;
		write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		write.descriptorCount = 1;
		write.pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(m_Device->GetLogical(), 1, &write, 0, nullptr);
	}

	void EnvironmentDescriptorSetManager::CreateDescriptorSets(VkDescriptorSetLayout layout, VkDescriptorPool pool)
	{
		std::vector<VkDescriptorSetLayout> layouts(Config::MAX_FRAMES_IN_FLIGHT, layout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = pool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(Config::MAX_FRAMES_IN_FLIGHT);
		allocInfo.pSetLayouts = layouts.data();

		m_DescriptorSets.resize(Config::MAX_FRAMES_IN_FLIGHT);

		if (vkAllocateDescriptorSets(m_Device->GetLogical(), &allocInfo, m_DescriptorSets.data()) != VK_SUCCESS)
			Core::Log::Error("Failed to allocate environment descriptor sets");
	}
}
