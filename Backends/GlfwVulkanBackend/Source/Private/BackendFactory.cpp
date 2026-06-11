#include "Core/BackendFactory.h"

#include "Glfw/Platform.h"
#include "Vulkan/Renderer.h"
#include "Vulkan/Config.h"

#include "Core/Log.h"

#include <volk.h>
#include <GLFW/glfw3.h>

#include <memory>
#include <vector>

namespace Nightbird::Core
{
	static Glfw::Platform* s_Platform = nullptr;

	std::unique_ptr<Core::Platform> CreatePlatform()
	{
		Log::Info("volkInitialize");
		volkInitialize();
		glfwInit();

		auto platform = std::make_unique<Glfw::Platform>();
		s_Platform = platform.get();
		return platform;
	}

	static VkSurfaceKHR CreateSurface(VkInstance instance)
	{
		VkSurfaceKHR surface;
		glfwCreateWindowSurface(instance, s_Platform->GetWindow(), nullptr, &surface);
		return surface;
	}

	std::unique_ptr<Core::Renderer> CreateRenderer()
	{
		std::vector<const char*> extensions = s_Platform->GetRequiredExtensions();

		if (Vulkan::Config::enableValidationLayers)
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		
		return std::make_unique<Vulkan::Renderer>(s_Platform, extensions, CreateSurface);
	}
}
