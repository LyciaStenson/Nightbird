#pragma once

#include "GX2/GX2Texture.h"

namespace Nightbird::Core
{
	class Texture;
	struct Material;
}

namespace Nightbird::GX2
{
	class Material
	{
	public:
		Material() = default;
		Material(const Core::Material& material, const Core::Texture& defaultTexture);

		Material(const Material&) = delete;
		Material& operator=(const Material&) = delete;
		Material(Material&&) = default;
		Material& operator=(Material&&) = default;

		const Texture& GetBaseColorTexture() const;
		const Texture& GetMetallicRoughnessTexture() const;
		const Texture& GetNormalTexture() const;

	private:
		Texture m_BaseColorTexture;
		Texture m_MetallicRoughnessTexture;
		Texture m_NormalTexture;
	};
}
