#include "GX2/GX2Texture.h"

#include "Core/Texture.h"
#include "Core/Log.h"

#include <coreinit/memdefaultheap.h>
#include <gx2/mem.h>
#include <gx2/surface.h>
#include <gx2/utils.h>

#include <cstring>

namespace Nightbird::GX2
{
	Texture::Texture(const Core::Texture& texture)
	{
		m_Texture.surface.dim = GX2_SURFACE_DIM_TEXTURE_2D;
		m_Texture.surface.width = texture.GetWidth();
		m_Texture.surface.height = texture.GetHeight();
		m_Texture.surface.depth = 1;
		m_Texture.surface.mipLevels = 1;
		m_Texture.surface.format = GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8;
		m_Texture.surface.aa = GX2_AA_MODE1X;
		m_Texture.surface.use = GX2_SURFACE_USE_TEXTURE;
		m_Texture.surface.tileMode = GX2_TILE_MODE_LINEAR_ALIGNED;
		m_Texture.surface.swizzle = 0;

		GX2CalcSurfaceSizeAndAlignment(&m_Texture.surface);

		m_ImageData = MEMAllocFromDefaultHeapEx(m_Texture.surface.imageSize, m_Texture.surface.alignment);
		if (!m_ImageData)
		{
			Core::Log::Error("Texture: Failed to allocate image memory");
			return;
		}

		switch (texture.GetFormat())
		{
		case Core::TextureFormat::RGBA8:
		{
			const auto& data = texture.GetData();
			memset(m_ImageData, 0, m_Texture.surface.imageSize);
			memcpy(m_ImageData, data.data(), data.size());
			break;
		}
		default:
			Core::Log::Error("Unsupported GX2 texture format");
			break;
		}

		m_Texture.surface.image = m_ImageData;

		m_Texture.viewFirstMip = 0;
		m_Texture.viewNumMips = 1;
		m_Texture.viewFirstSlice = 0;
		m_Texture.viewNumSlices = 1;
		m_Texture.compMap = GX2_COMP_MAP(GX2_SQ_SEL_R, GX2_SQ_SEL_G, GX2_SQ_SEL_B, GX2_SQ_SEL_A);

		GX2InitTextureRegs(&m_Texture);

		GX2Invalidate(GX2_INVALIDATE_MODE_CPU_TEXTURE, m_ImageData, m_Texture.surface.imageSize);

		GX2InitSampler(&m_Sampler, GX2_TEX_CLAMP_MODE_WRAP, GX2_TEX_XY_FILTER_MODE_LINEAR);
	}

	Texture::~Texture()
	{
		if (m_ImageData)
			MEMFreeToDefaultHeap(m_ImageData);
	}

	Texture::Texture(Texture&& other) noexcept
		: m_Texture(other.m_Texture), m_Sampler(other.m_Sampler), m_ImageData(other.m_ImageData)
	{
		other.m_ImageData = nullptr;
		other.m_Texture.surface.image = nullptr;
	}

	Texture& Texture::operator=(Texture&& other) noexcept
	{
		if (this != &other)
		{
			if (m_ImageData)
				MEMFreeToDefaultHeap(m_ImageData);

			m_Texture = other.m_Texture;
			m_Sampler = other.m_Sampler;
			m_ImageData = other.m_ImageData;

			other.m_ImageData = nullptr;
			other.m_Texture.surface.image = nullptr;
		}
		return *this;
	}

	const GX2Texture& Texture::GetTexture() const
	{
		return m_Texture;
	}

	const GX2Sampler& Texture::GetSampler() const
	{
		return m_Sampler;
	}
}
