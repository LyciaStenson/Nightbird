#pragma once

#include <volk.h>

#include <vector>

namespace Nightbird::Vulkan
{
	struct VertexLayout
	{
		VkVertexInputBindingDescription bindingDescription;
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

		static VertexLayout CreatePbrVertexLayout();
		static VertexLayout CreatePosVertexLayout();
	};
}
