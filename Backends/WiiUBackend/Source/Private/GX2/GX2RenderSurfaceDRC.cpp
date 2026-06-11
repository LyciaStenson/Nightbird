#include "GX2/GX2RenderSurfaceDRC.h"

#include <whb/gfx.h>

namespace Nightbird::GX2
{
	RenderSurfaceDRC::RenderSurfaceDRC()
	{

	}

	uint32_t RenderSurfaceDRC::GetWidth() const
	{
		return 854;
	}

	uint32_t RenderSurfaceDRC::GetHeight() const
	{
		return 480;
	}

	void RenderSurfaceDRC::Resize(uint32_t width, uint32_t height)
	{

	}

	void RenderSurfaceDRC::Begin()
	{
		WHBGfxBeginRenderDRC();
	}

	void RenderSurfaceDRC::Finish()
	{
		WHBGfxFinishRenderDRC();
	}
}