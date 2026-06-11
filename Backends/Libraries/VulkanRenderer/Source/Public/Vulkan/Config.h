#pragma once

#include <vector>

namespace Nightbird::Vulkan
{
	struct Config
	{
		static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

		static bool enableValidationLayers;

		static const std::vector<const char*> validationLayers;

		static const std::vector<const char*> deviceExtensions;

		static bool CheckValidationLayerSupport();

		static void Initialize();
	};
}