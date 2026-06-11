#include "Vulkan/Device.h"

#include "Vulkan/Config.h"
#include "Vulkan/SwapChainUtils.h"

#include "Core/Log.h"

#include <string>
#include <optional>
#include <map>
#include <set>

namespace Nightbird::Vulkan
{
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool IsComplete() const
		{
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
	{
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies)
		{
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				indices.graphicsFamily = i;

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

			if (presentSupport)
				indices.presentFamily = i;

			if (indices.IsComplete())
				break;

			i++;
		}

		return indices;
	}

	static bool CheckDeviceExtensionSupport(VkPhysicalDevice device)
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(Config::deviceExtensions.begin(), Config::deviceExtensions.end());

		for (const auto& extension : availableExtensions)
			requiredExtensions.erase(extension.extensionName);

		return requiredExtensions.empty();
	}

	static int RateDeviceSuitability(VkPhysicalDevice device, VkSurfaceKHR surface)
	{
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;

		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		int score = 0;

		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			score += 1000;

		score += deviceProperties.limits.maxImageDimension2D;

		QueueFamilyIndices indices = FindQueueFamilies(device, surface);
		if (!indices.IsComplete())
			return 0;

		if (!CheckDeviceExtensionSupport(device))
			return 0;

		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device, surface);
		if (swapChainSupport.formats.empty() || swapChainSupport.presentModes.empty())
			return 0;

		if (!deviceFeatures.samplerAnisotropy)
			return 0;

		return score;
	}

	Device::Device(VkInstance instance, VkSurfaceKHR surface)
		: m_Instance(instance), m_Surface(surface)
	{
		SelectPhysicalDevice();
		CreateLogicalDevice();
		CreateAllocator();
		CreateCommandPool();
		CreateCommandBuffers();
	}

	Device::~Device()
	{
		vmaDestroyAllocator(m_Allocator);

		vkDestroyCommandPool(m_LogicalDevice, m_CommandPool, nullptr);
		vkDestroyDevice(m_LogicalDevice, nullptr);
	}

	void Device::SelectPhysicalDevice()
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

		if (deviceCount == 0)
		{
			Core::Log::Error("Failed to select physical device: No devices found");
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

		std::multimap<int, VkPhysicalDevice> candidates;

		for (const auto& device : devices)
		{
			int score = RateDeviceSuitability(device, m_Surface);
			candidates.insert(std::make_pair(score, device));
		}

		if (candidates.rbegin()->first > 0)
		{
			m_PhysicalDevice = candidates.rbegin()->second;
		}
		else
		{
			Core::Log::Error("Failed to select physical device: No suitable device found");
		}
	}

	void Device::CreateLogicalDevice()
	{
		QueueFamilyIndices indices = FindQueueFamilies(m_PhysicalDevice, m_Surface);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures{};
		deviceFeatures.samplerAnisotropy = VK_TRUE;

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(Config::deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = Config::deviceExtensions.data();

		if (Config::enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(Config::validationLayers.size());
			createInfo.ppEnabledLayerNames = Config::validationLayers.data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
		}

		if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_LogicalDevice) != VK_SUCCESS)
		{
			Core::Log::Error("Failed to create logical device");
		}

		volkLoadDevice(m_LogicalDevice);

		m_GraphicsQueueFamily = indices.graphicsFamily.value();
		m_PresentQueueFamily = indices.presentFamily.value();

		vkGetDeviceQueue(m_LogicalDevice, m_GraphicsQueueFamily, 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_LogicalDevice, indices.presentFamily.value(), 0, &m_PresentQueue);
	}

	void Device::CreateAllocator()
	{
		VmaVulkanFunctions vulkanFunctions{};
		vulkanFunctions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
		vulkanFunctions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

		VmaAllocatorCreateInfo allocatorInfo{};
		allocatorInfo.physicalDevice = m_PhysicalDevice;
		allocatorInfo.device = m_LogicalDevice;
		allocatorInfo.instance = m_Instance;
		allocatorInfo.pVulkanFunctions = &vulkanFunctions;

		if (vmaCreateAllocator(&allocatorInfo, &m_Allocator) != VK_SUCCESS)
		{
			Core::Log::Error("Failed to create Vulkan Memory Allocator");
		}
	}

	void Device::CreateCommandPool()
	{
		QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(m_PhysicalDevice, m_Surface);

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

		if (vkCreateCommandPool(m_LogicalDevice, &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS)
		{
			Core::Log::Error("Failed to create command pool");
		}
	}

	void Device::CreateCommandBuffers()
	{
		m_CommandBuffers.resize(Config::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocateInfo.commandPool = m_CommandPool;
		allocateInfo.commandBufferCount = (uint32_t)m_CommandBuffers.size();

		if (vkAllocateCommandBuffers(m_LogicalDevice, &allocateInfo, m_CommandBuffers.data()) != VK_SUCCESS)
		{
			Core::Log::Error("Failed to allocate command buffers");
		}
	}

	uint32_t Device::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags propertyFlags)
	{
		VkPhysicalDeviceMemoryProperties memoryProperties;
		vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memoryProperties);

		for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
		{
			if (typeFilter & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & propertyFlags) == propertyFlags)
				return i;
		}

		Core::Log::Error("Failed to find a suitable memory type");
		return -1;
	}

	VkCommandBuffer Device::BeginSingleTimeCommands() const
	{
		VkCommandBufferAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocateInfo.commandPool = m_CommandPool;
		allocateInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(m_LogicalDevice, &allocateInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		return commandBuffer;
	}

	void Device::EndSingleTimeCommands(VkCommandBuffer commandBuffer) const
	{
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(m_GraphicsQueue);

		vkFreeCommandBuffers(m_LogicalDevice, m_CommandPool, 1, &commandBuffer);
	}

	VkDevice Device::GetLogical() const
	{
		return m_LogicalDevice;
	}

	VkPhysicalDevice Device::GetPhysical() const
	{
		return m_PhysicalDevice;
	}

	VmaAllocator Device::GetAllocator() const
	{
		return m_Allocator;
	}

	VkQueue Device::GetGraphicsQueue() const
	{
		return m_GraphicsQueue;
	}

	VkQueue Device::GetPresentQueue() const
	{
		return m_PresentQueue;
	}

	uint32_t Device::GetGraphicsQueueFamily() const
	{
		return m_GraphicsQueueFamily;
	}

	uint32_t Device::GetPresentQueueFamily() const
	{
		return m_PresentQueueFamily;
	}

	VkCommandBuffer Device::GetCommandBuffer(uint32_t currentFrame) const
	{
		return m_CommandBuffers[currentFrame];
	}

	VkFormat Device::FindSupportedDepthFormat() const
	{
		std::vector<VkFormat> candidates = {
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D24_UNORM_S8_UINT
		};

		for (VkFormat format : candidates)
		{
			VkFormatProperties properties;
			vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice, format, &properties);

			if (properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
				return format;
		}

		Core::Log::Error("No supported depth format found");
		return VK_FORMAT_UNDEFINED;
	}
}
