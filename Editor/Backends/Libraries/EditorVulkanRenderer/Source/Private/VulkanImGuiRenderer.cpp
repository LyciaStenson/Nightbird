#include "VulkanImGuiRenderer.h"

#include "Vulkan/Renderer.h"
#include "Vulkan/Device.h"
#include "Vulkan/SwapChain.h"

#include "Core/Log.h"

#include <imgui_impl_vulkan.h>

namespace Nightbird::Editor
{
	VulkanImGuiRenderer::VulkanImGuiRenderer(Vulkan::Renderer& renderer)
		: m_Renderer(renderer)
	{

	}

	void VulkanImGuiRenderer::Initialize()
	{
		CreateDescriptorPool();

		Vulkan::Device& device = m_Renderer.GetDevice();
		Vulkan::SwapChain& swapChain = m_Renderer.GetSwapChain();

		Core::RenderSurface& coreSurface = m_Renderer.GetDefaultSurface();
		auto& surface = static_cast<Vulkan::RenderSurface&>(coreSurface);
		Vulkan::RenderPass& renderPass = surface.GetRenderPass();

		VkInstance instance = m_Renderer.GetInstance().Get();

		ImGui_ImplVulkan_InitInfo initInfo = {};
		initInfo.ApiVersion = VK_API_VERSION_1_3;
		initInfo.Instance = instance;
		initInfo.PhysicalDevice = device.GetPhysical();
		initInfo.Device = device.GetLogical();
		initInfo.QueueFamily = device.GetGraphicsQueueFamily();
		initInfo.Queue = device.GetGraphicsQueue();
		initInfo.DescriptorPool = m_DescriptorPool;
		initInfo.MinImageCount = swapChain.GetMinImageCount();
		initInfo.ImageCount = swapChain.GetImageCount();
		initInfo.PipelineInfoMain.RenderPass = renderPass.Get();
		initInfo.PipelineInfoMain.Subpass = 0;
		initInfo.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

		ImGui_ImplVulkan_LoadFunctions(
			VK_API_VERSION_1_3,
			[](const char* functionName, void* vulkanInstance)
			{
				return vkGetInstanceProcAddr(*(reinterpret_cast<VkInstance*>(vulkanInstance)), functionName);
			},
			&instance
		);

		ImGui_ImplVulkan_Init(&initInfo);
	}

	void VulkanImGuiRenderer::Shutdown()
	{
		vkDeviceWaitIdle(m_Renderer.GetDevice().GetLogical());

		ImGui_ImplVulkan_Shutdown();

		vkDestroyDescriptorPool(m_Renderer.GetDevice().GetLogical(), m_DescriptorPool, nullptr);
	}

	void VulkanImGuiRenderer::NewFrame()
	{
		ImGui_ImplVulkan_NewFrame();
	}

	void VulkanImGuiRenderer::RenderDrawData()
	{
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_Renderer.GetCurrentCommandBuffer());
	}

	ImTextureID VulkanImGuiRenderer::RegisterSurface(Core::RenderSurface& surface)
	{
		auto& vulkanSurface = static_cast<Vulkan::OffscreenSurface&>(surface);
		return reinterpret_cast<ImTextureID>(ImGui_ImplVulkan_AddTexture(vulkanSurface.GetColorTexture().GetSampler(), vulkanSurface.GetColorTexture().GetImageView(), VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL));
	}
	
	void VulkanImGuiRenderer::UnregisterSurface(Core::RenderSurface& surface)
	{
		//ImGui_ImplVulkan_RemoveTexture()
	}

	void VulkanImGuiRenderer::CreateDescriptorPool()
	{
		VkDescriptorPoolSize poolSizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		poolInfo.maxSets = 1000 * IM_ARRAYSIZE(poolSizes);
		poolInfo.poolSizeCount = static_cast<uint32_t>(IM_ARRAYSIZE(poolSizes));
		poolInfo.pPoolSizes = poolSizes;

		if (vkCreateDescriptorPool(m_Renderer.GetDevice().GetLogical(), &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS)
		{
			Core::Log::Error("VulkanImGuiRenderer: Failed to create descriptor pool");
		}
	}
}
