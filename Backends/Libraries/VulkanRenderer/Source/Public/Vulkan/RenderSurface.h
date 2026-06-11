#pragma once

#include "Core/RenderSurface.h"

#include <volk.h>

#include <cstdint>

namespace Nightbird::Vulkan
{
	class RenderPass;
	
	enum class RenderSurfaceType
	{
		SwapChain, Offscreen
	};

	class RenderSurface : public Core::RenderSurface
	{
	public:
		virtual VkExtent2D GetExtent() const = 0;
		virtual RenderPass& GetRenderPass() const = 0;
		virtual bool NeedsResize() const = 0;
		virtual RenderSurfaceType GetSurfaceType() const = 0;
	};
}
