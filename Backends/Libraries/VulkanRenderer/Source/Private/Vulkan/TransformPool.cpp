#include "Vulkan/TransformPool.h"

#include "Vulkan/Device.h"
#include "Vulkan/DescriptorSetLayoutManager.h"
#include "Vulkan/Config.h"

#include "Core/Log.h"

#include <cstring>

namespace Nightbird::Vulkan
{
	struct alignas(16) TransformUBO
	{
		alignas(16) glm::mat4 model;
	};

	TransformPool::TransformPool(Device* device, VkDescriptorPool descriptorPool, DescriptorSetLayoutManager* descriptorSetLayoutManager, uint32_t capacity)
		: m_Device(device), m_Capacity(capacity)
	{
		m_UniformBuffers.reserve(capacity * Config::MAX_FRAMES_IN_FLIGHT);

		for (size_t i = 0; i < capacity * Config::MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_UniformBuffers.emplace_back(device, sizeof(TransformUBO), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		}

		std::vector<VkDescriptorSetLayout> layouts(capacity * Config::MAX_FRAMES_IN_FLIGHT, descriptorSetLayoutManager->GetMeshDescriptorSetLayout());

		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());
		allocInfo.pSetLayouts = layouts.data();

		// Allocate a descriptor set for each frame in flight
		m_DescriptorSets.resize(capacity * Config::MAX_FRAMES_IN_FLIGHT);
		if (vkAllocateDescriptorSets(device->GetLogical(), &allocInfo, m_DescriptorSets.data()) != VK_SUCCESS)
		{
			Core::Log::Error("Failed to allocate mesh instance descriptor sets");
			return;
		}

		// Update the descriptor set for each frame in flight
		for (size_t i = 0; i < capacity * Config::MAX_FRAMES_IN_FLIGHT; i++)
		{
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = m_UniformBuffers[i].Get();
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(TransformUBO);

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = m_DescriptorSets[i];
			descriptorWrite.dstBinding = 0;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;

			vkUpdateDescriptorSets(device->GetLogical(), 1, &descriptorWrite, 0, nullptr);
		}
	}

	VkDescriptorSet TransformPool::Acquire(const glm::mat4& transform, uint32_t currentFrame)
	{
		assert(m_CurrentIndex < m_Capacity && "TransformPool capacity exceeded");

		uint32_t bufferIndex = m_CurrentIndex * Config::MAX_FRAMES_IN_FLIGHT + currentFrame;

		TransformUBO transformUBO{};
		transformUBO.model = transform;
		memcpy(m_UniformBuffers[bufferIndex].GetMappedData(), &transformUBO, sizeof(transformUBO));

		VkDescriptorSet descriptorSet = m_DescriptorSets[bufferIndex];
		++m_CurrentIndex;
		return descriptorSet;
	}

	void TransformPool::Reset()
	{
		m_CurrentIndex = 0;
	}
}
