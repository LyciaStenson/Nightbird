#pragma once

#include <gx2/texture.h>
#include <gx2/sampler.h>

namespace Nightbird::Core
{
	class Texture;
}

namespace Nightbird::GX2
{
	class Texture
	{
	public:
		Texture() = default;
		Texture(const Core::Texture& texture);
		~Texture();

		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;
		Texture(Texture&& other) noexcept;
		Texture& operator=(Texture&& other) noexcept;

		const GX2Texture& GetTexture() const;
		const GX2Sampler& GetSampler() const;

	private:
		GX2Texture m_Texture = {};
		GX2Sampler m_Sampler = {};

		void* m_ImageData = nullptr;
	};
}
