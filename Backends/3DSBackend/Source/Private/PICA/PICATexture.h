#pragma once

#include <citro3d.h>
#include <3ds.h>

namespace Nightbird::Core
{
	class Texture;
}

namespace Nightbird::PICA
{
	class Texture
	{
	public:
		Texture() = default;
		Texture(const Core::Texture& texture);
		~Texture();

		void InitFromPixels(int width, int height, const uint8_t* pixels);

		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;
		Texture(Texture&& other) noexcept;
		Texture& operator=(Texture&& other) noexcept;

		C3D_Tex* GetTexture();

	private:
		C3D_Tex m_Texture = {};
		bool m_Initialized = false;
	};
}
