#pragma once

#include "Vulkan/VertexLayout.h"

#include <volk.h>

#include <string>
#include <vector>

namespace Nightbird::Vulkan
{
	class Device;
	class RenderPass;

	struct PipelineConfig
	{
		std::string vertexShaderName;
		std::string fragShaderName;
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
		bool depthTestEnable = true;
		bool depthWriteEnable = true;
		VkCompareOp depthCompareOp = VK_COMPARE_OP_LESS;
		VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT;
		bool blendEnable = false;
		VertexLayout vertexLayout;
	};

	class Pipeline
	{
	public:
		Pipeline(Device* device, RenderPass* renderPass, const PipelineConfig& config);
		~Pipeline();

		void Bind(VkCommandBuffer commandBuffer) const;

		VkPipelineLayout GetLayout() const;

	private:
		VkPipeline m_Pipeline;
		VkPipelineLayout m_PipelineLayout;

		Device* m_Device;

		void CreateGraphicsPipeline(RenderPass* renderPass, const PipelineConfig& config);
	};
}
