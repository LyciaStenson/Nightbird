#include "Vulkan/Config.h"

#include "Core/Log.h"

#include <volk.h>

#include <cstring>
#include <iostream>

namespace Nightbird::Vulkan
{
#ifdef NDEBUG
	bool Config::enableValidationLayers = false;
#else
	bool Config::enableValidationLayers = true;
#endif

	const std::vector<const char*> Config::validationLayers = { "VK_LAYER_KHRONOS_validation" };
	const std::vector<const char*> Config::deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	bool Config::CheckValidationLayerSupport()
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : validationLayers)
		{
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
				return false;
		}
		return true;
	}

	void Config::Initialize()
	{
		if (enableValidationLayers && !CheckValidationLayerSupport())
		{
			Core::Log::Warning("Validation layers requested but not available. Disabling validation layers for compatibility.");
			enableValidationLayers = false;
		}
	}
}
