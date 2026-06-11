#include "Vulkan/Material.h"

#include "Core/Material.h"

#include "Vulkan/DescriptorSetLayoutManager.h"
#include "Vulkan/Device.h"
#include "Vulkan/Config.h"

#include "Core/Log.h"

#include <glm/glm.hpp>

#include <array>

#include <cstring>

namespace Nightbird::Vulkan
{
	struct alignas(16) MaterialFactorsUBO
	{
		alignas(16) glm::vec4 baseColor;
		alignas(16) glm::vec3 metallicRoughness;
	};

	Material::Material(Device* device, const Core::Material& material, VkDescriptorPool descriptorPool, DescriptorSetLayoutManager* descriptorSetLayoutManager, const Core::Texture& defaultTexture)
	{
		CreateTextures(device, material, defaultTexture);
		CreateFactorsBuffer(device, material);
		CreateDescriptorSets(device, material, descriptorPool, descriptorSetLayoutManager);
	}

	void Material::CreateTextures(Device* device, const Core::Material& material, const Core::Texture& defaultTexture)
	{
		const Core::Texture& baseColorTexture = material.baseColorTexture ? static_cast<const Core::Texture&>(*material.baseColorTexture) : defaultTexture;
		m_BaseColorTexture = std::make_unique<Texture>(device, baseColorTexture, true);

		const Core::Texture& metallicRoughnessTexture = material.metallicRoughnessTexture ? static_cast<const Core::Texture&>(*material.metallicRoughnessTexture) : defaultTexture;
		m_MetallicRoughnessTexture = std::make_unique<Texture>(device, metallicRoughnessTexture, false);

		const Core::Texture& normalTexture = material.normalTexture ? static_cast<const Core::Texture&>(*material.normalTexture) : defaultTexture;
		m_NormalTexture = std::make_unique<Texture>(device, normalTexture, false);
	}

	void Material::CreateFactorsBuffer(Device* device, const Core::Material& material)
	{
		MaterialFactorsUBO factors{};
		factors.baseColor = material.baseColorFactor;
		factors.metallicRoughness = glm::vec3(0.0f, material.roughnessFactor, material.metallicFactor);

		m_FactorsUniformBuffer = std::make_unique<UniformBuffer>(device, sizeof(MaterialFactorsUBO), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		memcpy(m_FactorsUniformBuffer->GetMappedData(), &factors, sizeof(factors));
	}

	void Material::CreateDescriptorSets(Device* device, const Core::Material& material, VkDescriptorPool descriptorPool, DescriptorSetLayoutManager* descriptorSetLayoutManager)
	{
		std::vector<VkDescriptorSetLayout> layouts(Config::MAX_FRAMES_IN_FLIGHT, descriptorSetLayoutManager->GetMaterialDescriptorSetLayout());

		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());
		allocInfo.pSetLayouts = layouts.data();

		// Allocate a descriptor set for each frame in flight
		m_DescriptorSets.resize(Config::MAX_FRAMES_IN_FLIGHT);
		if (vkAllocateDescriptorSets(device->GetLogical(), &allocInfo, m_DescriptorSets.data()) != VK_SUCCESS)
		{
			Core::Log::Error("Failed to allocate mesh descriptor sets");
			return;
		}

		// Update the descriptor set for each frame in flight
		for (size_t i = 0; i < Config::MAX_FRAMES_IN_FLIGHT; i++)
		{
			VkDescriptorBufferInfo factorsBufferInfo{};
			factorsBufferInfo.buffer = m_FactorsUniformBuffer->Get();
			factorsBufferInfo.offset = 0;
			factorsBufferInfo.range = sizeof(MaterialFactorsUBO);

			VkDescriptorImageInfo baseColorInfo;
			baseColorInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			baseColorInfo.imageView = m_BaseColorTexture ? m_BaseColorTexture->GetImageView() : VK_NULL_HANDLE;
			baseColorInfo.sampler = m_BaseColorTexture ? m_BaseColorTexture->GetSampler() : VK_NULL_HANDLE;

			VkDescriptorImageInfo metallicRoughnessInfo;
			metallicRoughnessInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			metallicRoughnessInfo.imageView = m_MetallicRoughnessTexture ? m_MetallicRoughnessTexture->GetImageView() : VK_NULL_HANDLE;
			metallicRoughnessInfo.sampler = m_MetallicRoughnessTexture ? m_MetallicRoughnessTexture->GetSampler() : VK_NULL_HANDLE;

			VkDescriptorImageInfo normalInfo;
			normalInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			normalInfo.imageView = m_NormalTexture ? m_NormalTexture->GetImageView() : VK_NULL_HANDLE;
			normalInfo.sampler = m_NormalTexture ? m_NormalTexture->GetSampler() : VK_NULL_HANDLE;

			std::array<VkWriteDescriptorSet, 4> descriptorWrites{};

			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = m_DescriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &factorsBufferInfo;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = m_DescriptorSets[i];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pImageInfo = &baseColorInfo;

			descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[2].dstSet = m_DescriptorSets[i];
			descriptorWrites[2].dstBinding = 2;
			descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[2].descriptorCount = 1;
			descriptorWrites[2].pImageInfo = &metallicRoughnessInfo;

			descriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[3].dstSet = m_DescriptorSets[i];
			descriptorWrites[3].dstBinding = 3;
			descriptorWrites[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[3].descriptorCount = 1;
			descriptorWrites[3].pImageInfo = &normalInfo;

			vkUpdateDescriptorSets(device->GetLogical(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}
	}

	const std::vector<VkDescriptorSet>& Material::GetDescriptorSets() const
	{
		return m_DescriptorSets;
	}
}
