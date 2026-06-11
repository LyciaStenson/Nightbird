#include "Vulkan/FrameDescriptorSetManager.h"

#include "Vulkan/Config.h"
#include "Vulkan/Device.h"
#include "Vulkan/Buffer.h"
#include "Vulkan/UniformBuffer.h"
#include "Vulkan/CameraUBO.h"
#include "Vulkan/LightData.h"

#include "Core/Log.h"

#include <array>
#include <cstring>

namespace Nightbird::Vulkan
{
	FrameDescriptorSetManager::FrameDescriptorSetManager(Device* device, VkDescriptorSetLayout layout, VkDescriptorPool pool)
		: m_Device(device)
	{
		CreateBuffers();
		CreateDescriptorSets(layout, pool);
	}
	
	const std::vector<VkDescriptorSet>& FrameDescriptorSetManager::GetDescriptorSets() const
	{
		return m_DescriptorSets;
	}

	void FrameDescriptorSetManager::UpdateCamera(uint32_t frameIndex, const CameraUBO& cameraUBO)
	{
		memcpy(m_CameraBuffers[frameIndex].GetMappedData(), &cameraUBO, sizeof(cameraUBO));
	}

	void FrameDescriptorSetManager::UpdateDirectionalLights(uint32_t frameIndex, const std::vector<DirectionalLightData>& directionalLights)
	{
		VkDeviceSize size = sizeof(PointLightData) * directionalLights.size();
		if (size > 0)
		{
			m_DirectionalLightBuffers[frameIndex].UploadData(directionalLights.data(), size);

			DirectionalLightMetaUBO metaUBO{};
			metaUBO.count = static_cast<uint32_t>(directionalLights.size());
			memcpy(m_DirectionalLightMetaBuffers[frameIndex].GetMappedData(), &metaUBO, sizeof(metaUBO));
		}
	}

	void FrameDescriptorSetManager::UpdatePointLights(uint32_t frameIndex, const std::vector<PointLightData>& pointLights)
	{
		VkDeviceSize size = sizeof(PointLightData) * pointLights.size();
		if (size > 0)
		{
			m_PointLightBuffers[frameIndex].UploadData(pointLights.data(), size);

			PointLightMetaUBO metaUBO{};
			metaUBO.count = static_cast<uint32_t>(pointLights.size());
			memcpy(m_PointLightMetaBuffers[frameIndex].GetMappedData(), &metaUBO, sizeof(metaUBO));
		}
	}
	
	void FrameDescriptorSetManager::CreateBuffers()
	{
		VkDeviceSize cameraSize = sizeof(CameraUBO);
		VkDeviceSize directionalLightBufferSize = sizeof(DirectionalLightData) * MAX_DIRECTIONAL_LIGHTS;
		VkDeviceSize directionalLightMetaBufferSize = sizeof(DirectionalLightMetaUBO) * MAX_DIRECTIONAL_LIGHTS;
		VkDeviceSize pointLightBufferSize = sizeof(PointLightData) * MAX_POINT_LIGHTS;
		VkDeviceSize pointLightMetaBufferSize = sizeof(PointLightMetaUBO) * MAX_POINT_LIGHTS;

		m_CameraBuffers.reserve(Config::MAX_FRAMES_IN_FLIGHT);
		m_DirectionalLightBuffers.reserve(Config::MAX_FRAMES_IN_FLIGHT);
		m_DirectionalLightMetaBuffers.reserve(Config::MAX_FRAMES_IN_FLIGHT);
		m_PointLightBuffers.reserve(Config::MAX_FRAMES_IN_FLIGHT);
		m_PointLightMetaBuffers.reserve(Config::MAX_FRAMES_IN_FLIGHT);

		for (size_t i = 0; i < Config::MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_CameraBuffers.emplace_back(m_Device, cameraSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			m_DirectionalLightBuffers.emplace_back(m_Device, directionalLightBufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			m_DirectionalLightMetaBuffers.emplace_back(m_Device, directionalLightMetaBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			m_PointLightBuffers.emplace_back(m_Device, pointLightBufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			m_PointLightMetaBuffers.emplace_back(m_Device, pointLightMetaBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		}
	}

	void FrameDescriptorSetManager::CreateDescriptorSets(VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool)
	{
		VkDevice logicalDevice = m_Device->GetLogical();

		std::vector<VkDescriptorSetLayout> layouts(Config::MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(Config::MAX_FRAMES_IN_FLIGHT);
		allocInfo.pSetLayouts = layouts.data();

		m_DescriptorSets.resize(Config::MAX_FRAMES_IN_FLIGHT);
		if (vkAllocateDescriptorSets(logicalDevice, &allocInfo, m_DescriptorSets.data()) != VK_SUCCESS)
		{
			Core::Log::Error("Failed to allocate frame descriptor sets");
			return;
		}

		for (size_t i = 0; i < Config::MAX_FRAMES_IN_FLIGHT; i++)
		{
			VkDescriptorBufferInfo cameraInfo{};
			cameraInfo.buffer = m_CameraBuffers[i].Get();
			cameraInfo.offset = 0;
			cameraInfo.range = sizeof(CameraUBO);

			VkDescriptorBufferInfo directionalLightsInfo{};
			directionalLightsInfo.buffer = m_DirectionalLightBuffers[i].Get();
			directionalLightsInfo.offset = 0;
			directionalLightsInfo.range = VK_WHOLE_SIZE;
			//directionalLightsInfo.range = sizeof(PointLightData) * MAX_DIRECTIONAL_LIGHTS;

			VkDescriptorBufferInfo directionalLightsMetaInfo{};
			directionalLightsMetaInfo.buffer = m_DirectionalLightMetaBuffers[i].Get();
			directionalLightsMetaInfo.offset = 0;
			directionalLightsMetaInfo.range = sizeof(DirectionalLightMetaUBO);

			VkDescriptorBufferInfo pointLightsInfo{};
			pointLightsInfo.buffer = m_PointLightBuffers[i].Get();
			pointLightsInfo.offset = 0;
			pointLightsInfo.range = VK_WHOLE_SIZE;
			//pointLightsInfo.range = sizeof(PointLightData) * MAX_POINT_LIGHTS;

			VkDescriptorBufferInfo pointLightsMetaInfo{};
			pointLightsMetaInfo.buffer = m_PointLightMetaBuffers[i].Get();
			pointLightsMetaInfo.offset = 0;
			pointLightsMetaInfo.range = sizeof(PointLightMetaUBO);

			std::array<VkWriteDescriptorSet, 5> descriptorWrites{};
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = m_DescriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &cameraInfo;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = m_DescriptorSets[i];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pBufferInfo = &directionalLightsInfo;

			descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[2].dstSet = m_DescriptorSets[i];
			descriptorWrites[2].dstBinding = 2;
			descriptorWrites[2].dstArrayElement = 0;
			descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[2].descriptorCount = 1;
			descriptorWrites[2].pBufferInfo = &directionalLightsMetaInfo;

			descriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[3].dstSet = m_DescriptorSets[i];
			descriptorWrites[3].dstBinding = 3;
			descriptorWrites[3].dstArrayElement = 0;
			descriptorWrites[3].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			descriptorWrites[3].descriptorCount = 1;
			descriptorWrites[3].pBufferInfo = &pointLightsInfo;

			descriptorWrites[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[4].dstSet = m_DescriptorSets[i];
			descriptorWrites[4].dstBinding = 4;
			descriptorWrites[4].dstArrayElement = 0;
			descriptorWrites[4].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[4].descriptorCount = 1;
			descriptorWrites[4].pBufferInfo = &pointLightsMetaInfo;

			vkUpdateDescriptorSets(logicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}
	}
}
