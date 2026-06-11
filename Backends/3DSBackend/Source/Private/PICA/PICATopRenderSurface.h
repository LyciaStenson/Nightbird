#pragma once

#include "Core/RenderSurface.h"

#include <citro3d.h>

namespace Nightbird::PICA
{
	class TopRenderSurface : public Core::RenderSurface
	{
	public:
		TopRenderSurface();
		~TopRenderSurface();

		uint32_t GetWidth() const override;
		uint32_t GetHeight() const override;

		void Resize(uint32_t width, uint32_t height) override;

		void Begin();

	private:
		C3D_RenderTarget* m_Target = nullptr;
	};
}
