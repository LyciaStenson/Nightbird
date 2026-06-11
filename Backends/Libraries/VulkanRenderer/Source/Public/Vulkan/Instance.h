#pragma once

#include <vector>

#include <volk.h>

namespace Nightbird::Vulkan
{
	class Instance
	{
	public:
		Instance(std::vector<const char*> extensions);
		~Instance();

		VkInstance Get() const;

	private:
		VkInstance instance = VK_NULL_HANDLE;
		
		VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;

		void CreateInstance(std::vector<const char*> extensions);

		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

		void SetupDebugMessenger();

		VkResult CreateDebugUtilsMessengerEXT(
			VkInstance instance,
			const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
			const VkAllocationCallbacks* pAllocator,
			VkDebugUtilsMessengerEXT* pDebugMessenger);

		void DestroyDebugUtilsMessengerEXT(
			VkInstance instance,
			VkDebugUtilsMessengerEXT debugMessenger,
			const VkAllocationCallbacks* pAllocator);

		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);
	};
}