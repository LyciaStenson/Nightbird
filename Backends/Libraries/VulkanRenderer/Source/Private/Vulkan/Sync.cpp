#include "Vulkan/Sync.h"

#include "Vulkan/Config.h"

#include <iostream>

namespace Nightbird::Vulkan
{
	Sync::Sync(VkDevice logicalDevice)
		: m_LogicalDevice(logicalDevice)
	{
		CreateSyncObjects();
	}

	Sync::~Sync()
	{
		CleanupSyncObjects();
	}

	void Sync::CreateSyncObjects()
	{
		m_ImageAvailableSemaphores.resize(Config::MAX_FRAMES_IN_FLIGHT);
		m_RenderFinishedSemaphores.resize(Config::MAX_FRAMES_IN_FLIGHT);
		m_InFlightFences.resize(Config::MAX_FRAMES_IN_FLIGHT);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < Config::MAX_FRAMES_IN_FLIGHT; i++)
		{
			if (
				vkCreateSemaphore(m_LogicalDevice, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]) != VK_SUCCESS ||
				vkCreateSemaphore(m_LogicalDevice, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]) != VK_SUCCESS ||
				vkCreateFence(m_LogicalDevice, &fenceInfo, nullptr, &m_InFlightFences[i]) != VK_SUCCESS
				)
			{
				std::cerr << "Failed to create semaphores and fence" << std::endl;
				return;
			}
		}
	}

	void Sync::CleanupSyncObjects()
	{
		for (size_t i = 0; i < Config::MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkDestroySemaphore(m_LogicalDevice, m_RenderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(m_LogicalDevice, m_ImageAvailableSemaphores[i], nullptr);
			vkDestroyFence(m_LogicalDevice, m_InFlightFences[i], nullptr);
		}

		m_RenderFinishedSemaphores.clear();
		m_ImageAvailableSemaphores.clear();
		m_InFlightFences.clear();
	}
}