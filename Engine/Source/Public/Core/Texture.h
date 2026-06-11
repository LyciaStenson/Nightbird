#pragma once

#include <cstdint>
#include <vector>

namespace Nightbird::Core
{
	enum class TextureFormat : uint32_t
	{
		RGBA8 = 0,
		T3X = 1
	};

	class Texture
	{
	public:
		Texture(uint32_t width, uint32_t height, TextureFormat format, std::vector<uint8_t> data);

		uint32_t GetWidth() const;
		uint32_t GetHeight() const;

		TextureFormat GetFormat() const;
		const std::vector<uint8_t>& GetData() const;

	private:
		uint32_t m_Width;
		uint32_t m_Height;

		TextureFormat m_Format;
		std::vector<uint8_t> m_Data;
	};
}
