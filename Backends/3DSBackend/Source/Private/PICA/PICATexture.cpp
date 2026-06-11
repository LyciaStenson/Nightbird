#include "PICA/PICATexture.h"

#include "Core/Texture.h"
#include "Core/Log.h"

#include <tex3ds.h>

namespace Nightbird::PICA
{
	Texture::Texture(const Core::Texture& texture)
	{
		switch (texture.GetFormat())
		{
		case Core::TextureFormat::T3X:
		{
			const auto& data = texture.GetData();

			if (data.empty())
			{
				Core::Log::Error("PICA::Texture: No texture data");
				return;
			}

			void* linearData = linearAlloc(data.size());
			if (!linearData)
			{
				Core::Log::Error("PICA::Texture: Failed to allocate linear memory");
				return;
			}

			memcpy(linearData, data.data(), data.size());
			Tex3DS_Texture t3x = Tex3DS_TextureImport(linearData, data.size(), &m_Texture, nullptr, false);
			linearFree(linearData);

			if (!t3x)
			{
				Core::Log::Error("PICA::Texture: Failed to import texture");
				return;
			}

			Tex3DS_TextureFree(t3x);

			C3D_TexSetWrap(&m_Texture, GPU_REPEAT, GPU_REPEAT);
			C3D_TexSetFilter(&m_Texture, GPU_LINEAR, GPU_NEAREST);

			m_Initialized = true;

			break;
		}
		default:
			Core::Log::Error("Unsupported PICA texture format: " + std::to_string((uint32_t)texture.GetFormat()));
			return;
		}
	}

	Texture::~Texture()
	{
		if (m_Initialized)
			C3D_TexDelete(&m_Texture);
	}

	void Texture::InitFromPixels(int width, int height, const uint8_t* pixels)
	{
		if (!pixels || width <= 0 || height <= 0) {
			Core::Log::Error("PICA::Texture: Invalid pixel data or size");
			return;
		}

		C3D_TexInit(&m_Texture, width, height, GPU_RGBA8);
		C3D_TexUpload(&m_Texture, pixels);
		GSPGPU_FlushDataCache(m_Texture.data, m_Texture.size);

		C3D_TexSetWrap(&m_Texture, GPU_CLAMP_TO_EDGE, GPU_CLAMP_TO_EDGE);
		C3D_TexSetFilter(&m_Texture, GPU_LINEAR, GPU_NEAREST);

		m_Initialized = true;
	}

	Texture::Texture(Texture&& other) noexcept
		: m_Texture(other.m_Texture), m_Initialized(other.m_Initialized)
	{
		other.m_Initialized = false;
	}

	Texture& Texture::operator=(Texture&& other) noexcept
	{
		if (this != &other)
		{
			if (m_Initialized)
				C3D_TexDelete(&m_Texture);

			m_Texture = other.m_Texture;
			m_Initialized = other.m_Initialized;

			other.m_Initialized = false;
		}
		return *this;
	}

	C3D_Tex* Texture::GetTexture()
	{
		return &m_Texture;
	}
}
