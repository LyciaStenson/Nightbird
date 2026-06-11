#pragma once

#include "Vulkan/StorageBuffer.h"

#include <volk.h>

#include <vector>

namespace Nightbird::Vulkan
{
	class Device;
	class UniformBuffer;
	struct CameraUBO;
	struct DirectionalLightData;
	struct DirectionalLightMetaUBO;
	struct PointLightData;
	struct PointLightMetaUBO;

	class FrameDescriptorSetManager
	{
	public:
		FrameDescriptorSetManager(Device* device, VkDescriptorSetLayout layout, VkDescriptorPool pool);
		
		const std::vector<VkDescriptorSet>& GetDescriptorSets() const;

		void UpdateCamera(uint32_t frameIndex, const CameraUBO& cameraUBO);
		void UpdateDirectionalLights(uint32_t frameIndex, const std::vector<DirectionalLightData>& directionalLights);
		void UpdatePointLights(uint32_t frameIndex, const std::vector<PointLightData>& pointLights);

	private:
		Device* m_Device;

		std::vector<VkDescriptorSet> m_DescriptorSets;

		std::vector<UniformBuffer> m_CameraBuffers;
		std::vector<StorageBuffer> m_DirectionalLightBuffers;
		std::vector<UniformBuffer> m_DirectionalLightMetaBuffers;
		std::vector<StorageBuffer> m_PointLightBuffers;
		std::vector<UniformBuffer> m_PointLightMetaBuffers;

		void CreateBuffers();
		void CreateDescriptorSets(VkDescriptorSetLayout layout, VkDescriptorPool pool);
	};
}