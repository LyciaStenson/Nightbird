#pragma once

#include <volk.h>

namespace Nightbird::Vulkan
{
	class Device;

	class DescriptorSetLayoutManager
	{
	public:
		DescriptorSetLayoutManager(Device* device);
		~DescriptorSetLayoutManager();
		
		VkDescriptorSetLayout GetFrameDescriptorSetLayout() const;
		VkDescriptorSetLayout GetEnvironmentDescriptorSetLayout() const;
		VkDescriptorSetLayout GetMeshDescriptorSetLayout() const;
		VkDescriptorSetLayout GetMaterialDescriptorSetLayout() const;
	
	private:
		void CreateFrameDescriptorSetLayout();
		void CreateEnvironmentDescriptorSetLayout();
		void CreateMeshDescriptorSetLayout();
		void CreateMaterialDescriptorSetLayout();
		
		VkDescriptorSetLayout m_FrameDescriptorSetLayout;
		VkDescriptorSetLayout m_EnvironmentDescriptorSetLayout;
		VkDescriptorSetLayout m_MeshDescriptorSetLayout;
		VkDescriptorSetLayout m_MaterialDescriptorSetLayout;

		Device* m_Device;
	};
}