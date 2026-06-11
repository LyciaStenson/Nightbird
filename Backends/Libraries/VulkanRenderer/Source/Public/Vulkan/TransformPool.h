#pragma once

#include "UniformBuffer.h"

#include <volk.h>
#include <glm/glm.hpp>

#include <vector>

namespace Nightbird::Vulkan
{
	class Device;
	class DescriptorSetLayoutManager;

	class TransformPool
	{
	public:
		TransformPool(Device* device, VkDescriptorPool descriptorPool, DescriptorSetLayoutManager* descriptorSetLayoutManager, uint32_t capacity);

		VkDescriptorSet Acquire(const glm::mat4& transform, uint32_t currentFrame);

		void Reset();

	private:
		Device* m_Device;

		uint32_t m_Capacity;
		uint32_t m_CurrentIndex = 0;

		std::vector<UniformBuffer> m_UniformBuffers;
		std::vector<VkDescriptorSet> m_DescriptorSets;
	};
}