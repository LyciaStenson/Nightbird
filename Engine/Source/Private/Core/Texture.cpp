#include "Core/Texture.h"

namespace Nightbird::Core
{
	Texture::Texture(uint32_t width, uint32_t height, TextureFormat format, std::vector<uint8_t> data)
		: m_Width(width), m_Height(height), m_Format(format), m_Data(std::move(data))
	{

	}

	uint32_t Texture::GetWidth() const
	{
		return m_Width;
	}

	uint32_t Texture::GetHeight() const
	{
		return m_Height;
	}

	TextureFormat Texture::GetFormat() const
	{
		return m_Format;
	}

	const std::vector<uint8_t>& Texture::GetData() const
	{
		return m_Data;
	}
}
