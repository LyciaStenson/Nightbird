#include "GX2/GX2Material.h"

#include "Core/Material.h"
#include "Core/Texture.h"

namespace Nightbird::GX2
{
	Material::Material(const Core::Material& material, const Core::Texture& defaultTexture)
	{
		const Core::Texture& baseColorTexture = material.baseColorTexture ? *material.baseColorTexture : defaultTexture;
		const Core::Texture& metallicRoughnessTexture = material.metallicRoughnessTexture ? *material.metallicRoughnessTexture : defaultTexture;
		const Core::Texture& normalTexture = material.normalTexture ? *material.normalTexture : defaultTexture;

		m_BaseColorTexture = Texture(baseColorTexture);
		m_MetallicRoughnessTexture = Texture(metallicRoughnessTexture);
		m_NormalTexture = Texture(normalTexture);
	}

	const Texture& Material::GetBaseColorTexture() const
	{
		return m_BaseColorTexture;
	}

	const Texture& Material::GetMetallicRoughnessTexture() const
	{
		return m_MetallicRoughnessTexture;
	}

	const Texture& Material::GetNormalTexture() const
	{
		return m_NormalTexture;
	}
}
