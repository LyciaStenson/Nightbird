#include "PICA/PICATopRenderSurface.h"

#define CLEAR_COLOR 0x000000FF
#define DISPLAY_TRANSFER_FLAGS \
	(GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
	GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
	GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

namespace Nightbird::PICA
{
	TopRenderSurface::TopRenderSurface()
	{
		m_Target = C3D_RenderTargetCreate(GetWidth(), GetHeight(), GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
		C3D_RenderTargetSetOutput(m_Target, GFX_TOP, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);
	}

	TopRenderSurface::~TopRenderSurface()
	{
		C3D_RenderTargetDelete(m_Target);
	}

	uint32_t TopRenderSurface::GetWidth() const
	{
		return 240;
	}

	uint32_t TopRenderSurface::GetHeight() const
	{
		return 400;
	}

	void TopRenderSurface::Resize(uint32_t width, uint32_t height)
	{

	}

	void TopRenderSurface::Begin()
	{
		C3D_RenderTargetClear(m_Target, C3D_CLEAR_ALL, CLEAR_COLOR, 0);
		C3D_FrameDrawOn(m_Target);
	}
}