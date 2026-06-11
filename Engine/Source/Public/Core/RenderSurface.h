#pragma once

#include <cstdint>

namespace Nightbird::Core
{
	enum class RenderSurfaceFormat
	{
		Default,
		DepthOnly
	};

	class RenderSurface
	{
	public:
		virtual ~RenderSurface() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;
	};
}
