#pragma once

#include <volk.h>

#include <vector>

namespace Nightbird::Vulkan
{
	class Sync
	{
	public:
		Sync(VkDevice logicalDevice);
		~Sync();

		void CreateSyncObjects();
		void CleanupSyncObjects();

		std::vector<VkSemaphore> m_ImageAvailableSemaphores;
		std::vector<VkSemaphore> m_RenderFinishedSemaphores;
		std::vector<VkFence> m_InFlightFences;

	private:
		VkDevice m_LogicalDevice;
	};
}