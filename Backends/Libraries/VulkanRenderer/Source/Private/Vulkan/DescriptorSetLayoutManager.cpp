#include "Vulkan/DescriptorSetLayoutManager.h"

#include "Vulkan/Device.h"

#include <iostream>
#include <array>

namespace Nightbird::Vulkan
{
	DescriptorSetLayoutManager::DescriptorSetLayoutManager(Device* device)
		: m_Device(device)
	{
		CreateEnvironmentDescriptorSetLayout();
		CreateFrameDescriptorSetLayout();
		CreateMeshDescriptorSetLayout();
		CreateMaterialDescriptorSetLayout();
	}

	DescriptorSetLayoutManager::~DescriptorSetLayoutManager()
	{
		vkDestroyDescriptorSetLayout(m_Device->GetLogical(), m_EnvironmentDescriptorSetLayout, nullptr);
		vkDestroyDescriptorSetLayout(m_Device->GetLogical(), m_FrameDescriptorSetLayout, nullptr);
		vkDestroyDescriptorSetLayout(m_Device->GetLogical(), m_MeshDescriptorSetLayout, nullptr);
		vkDestroyDescriptorSetLayout(m_Device->GetLogical(), m_MaterialDescriptorSetLayout, nullptr);
	}
	
	VkDescriptorSetLayout DescriptorSetLayoutManager::GetFrameDescriptorSetLayout() const
	{
		return m_FrameDescriptorSetLayout;
	}
	
	VkDescriptorSetLayout DescriptorSetLayoutManager::GetEnvironmentDescriptorSetLayout() const
	{
		return m_EnvironmentDescriptorSetLayout;
	}
	
	VkDescriptorSetLayout DescriptorSetLayoutManager::GetMeshDescriptorSetLayout() const
	{
		return m_MeshDescriptorSetLayout;
	}

	VkDescriptorSetLayout DescriptorSetLayoutManager::GetMaterialDescriptorSetLayout() const
	{
		return m_MaterialDescriptorSetLayout;
	}

	void DescriptorSetLayoutManager::CreateFrameDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding cameraBinding{};
		cameraBinding.binding = 0;
		cameraBinding.descriptorCount = 1;
		cameraBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		cameraBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutBinding directionalLightsBinding{};
		directionalLightsBinding.binding = 1;
		directionalLightsBinding.descriptorCount = 1;
		directionalLightsBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		directionalLightsBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutBinding directionalLightsMetaBinding{};
		directionalLightsMetaBinding.binding = 2;
		directionalLightsMetaBinding.descriptorCount = 1;
		directionalLightsMetaBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		directionalLightsMetaBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutBinding pointLightsBinding{};
		pointLightsBinding.binding = 3;
		pointLightsBinding.descriptorCount = 1;
		pointLightsBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		pointLightsBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutBinding pointLightsMetaBinding{};
		pointLightsMetaBinding.binding = 4;
		pointLightsMetaBinding.descriptorCount = 1;
		pointLightsMetaBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		pointLightsMetaBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		std::array<VkDescriptorSetLayoutBinding, 5> bindings = {
			cameraBinding, directionalLightsBinding, directionalLightsMetaBinding,
			pointLightsBinding, pointLightsMetaBinding
		};

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(m_Device->GetLogical(), &layoutInfo, nullptr, &m_FrameDescriptorSetLayout) != VK_SUCCESS)
		{
			std::cerr << "Failed to create global descriptor set layout" << std::endl;
		}
	}

	void DescriptorSetLayoutManager::CreateEnvironmentDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding skyboxBinding{};
		skyboxBinding.binding = 0;
		skyboxBinding.descriptorCount = 1;
		skyboxBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		skyboxBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &skyboxBinding;

		if (vkCreateDescriptorSetLayout(m_Device->GetLogical(), &layoutInfo, nullptr, &m_EnvironmentDescriptorSetLayout) != VK_SUCCESS)
		{
			std::cerr << "Failed to create environment descriptor set layout" << std::endl;
		}
	}
	
	void DescriptorSetLayoutManager::CreateMeshDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding uboBinding{};
		uboBinding.binding = 0;
		uboBinding.descriptorCount = 1;
		uboBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &uboBinding;

		if (vkCreateDescriptorSetLayout(m_Device->GetLogical(), &layoutInfo, nullptr, &m_MeshDescriptorSetLayout) != VK_SUCCESS)
		{
			std::cerr << "Failed to create mesh descriptor set layout" << std::endl;
		}
	}

	void DescriptorSetLayoutManager::CreateMaterialDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding factorsBinding{};
		factorsBinding.binding = 0;
		factorsBinding.descriptorCount = 1;
		factorsBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		factorsBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutBinding baseColorBinding{};
		baseColorBinding.binding = 1;
		baseColorBinding.descriptorCount = 1;
		baseColorBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		baseColorBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutBinding metallicRoughnessBinding{};
		metallicRoughnessBinding.binding = 2;
		metallicRoughnessBinding.descriptorCount = 1;
		metallicRoughnessBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		metallicRoughnessBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutBinding normalBinding{};
		normalBinding.binding = 3;
		normalBinding.descriptorCount = 1;
		normalBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		normalBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		std::array<VkDescriptorSetLayoutBinding, 4> bindings = { factorsBinding, baseColorBinding, metallicRoughnessBinding, normalBinding };
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(m_Device->GetLogical(), &layoutInfo, nullptr, &m_MaterialDescriptorSetLayout) != VK_SUCCESS)
		{
			std::cerr << "Failed to create material descriptor set layout" << std::endl;
		}
	}
}