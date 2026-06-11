#include "Vulkan/VertexLayout.h"

#include "Core/Vertex.h"

namespace Nightbird::Vulkan
{
	VertexLayout VertexLayout::CreatePbrVertexLayout()
	{
		VertexLayout layout{};

		layout.bindingDescription.binding = 0;
		layout.bindingDescription.stride = sizeof(Core::Vertex);
		layout.bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		layout.attributeDescriptions = {
			{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT,	offsetof(Core::Vertex, position) },
			{ 1, 0, VK_FORMAT_R32G32B32_SFLOAT,	offsetof(Core::Vertex, normal) },
			{ 2, 0, VK_FORMAT_R32G32_SFLOAT,	offsetof(Core::Vertex, baseColorTexCoord) },
			{ 3, 0, VK_FORMAT_R32G32_SFLOAT,	offsetof(Core::Vertex, metallicRoughnessTexCoord) },
			{ 4, 0, VK_FORMAT_R32G32_SFLOAT,	offsetof(Core::Vertex, normalTexCoord) }
		};

		return layout;
	}

	VertexLayout VertexLayout::CreatePosVertexLayout()
	{
		VertexLayout layout{};

		layout.bindingDescription.binding = 0;
		layout.bindingDescription.stride = sizeof(Core::VertexPos);
		layout.bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		layout.attributeDescriptions = {
			{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT,	offsetof(Core::Vertex, position) }
		};

		return layout;
	}
}
