#include "GX2/GX2RenderSurfaceTV.h"

#include <whb/gfx.h>

namespace Nightbird::GX2
{
	RenderSurfaceTV::RenderSurfaceTV()
	{

	}

	uint32_t RenderSurfaceTV::GetWidth() const
	{
		return WHBGfxGetTVColourBuffer()->surface.width;
	}

	uint32_t RenderSurfaceTV::GetHeight() const
	{
		return WHBGfxGetTVColourBuffer()->surface.height;
	}
	
	void RenderSurfaceTV::Resize(uint32_t width, uint32_t height)
	{

	}
	
	void RenderSurfaceTV::Begin()
	{
		WHBGfxBeginRenderTV();
	}
	
	void RenderSurfaceTV::Finish()
	{
		WHBGfxFinishRenderTV();
	}
}