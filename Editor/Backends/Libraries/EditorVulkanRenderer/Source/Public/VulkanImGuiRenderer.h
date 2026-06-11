#pragma once

#include "ImGuiRenderer.h"

#include <volk.h>

namespace Nightbird::Vulkan
{
	class Renderer;
	class Device;
	class SwapChain;
}

namespace Nightbird::Editor
{
	class VulkanImGuiRenderer : public ImGuiRenderer
	{
	public:
		VulkanImGuiRenderer(Vulkan::Renderer& renderer);

		void Initialize() override;
		void Shutdown() override;
		void NewFrame() override;
		void RenderDrawData() override;
		ImTextureID RegisterSurface(Core::RenderSurface& surface);
		void UnregisterSurface(Core::RenderSurface& surface);

	private:
		Vulkan::Renderer& m_Renderer;

		VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;

		void CreateDescriptorPool();
	};
}
