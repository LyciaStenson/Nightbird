#pragma once

#include "GX2/GX2RenderSurface.h"

namespace Nightbird::GX2
{
	class RenderSurfaceTV : public RenderSurface
	{
	public:
		RenderSurfaceTV();

		uint32_t GetWidth() const override;
		uint32_t GetHeight() const override;

		void Resize(uint32_t width, uint32_t height) override;

		void Begin() override;
		void Finish() override;
	};
}
