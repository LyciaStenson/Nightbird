#include "PICA/PICAMaterial.h"

#include "Core/Material.h"
#include "Core/Texture.h"

namespace Nightbird::PICA
{
	Material::Material(const Core::Material& material, std::shared_ptr<Texture> baseColorTexture)
		: m_BaseColorTexture(baseColorTexture)
	{

	}

	std::shared_ptr<Texture> Material::GetBaseColorTexture()
	{
		return m_BaseColorTexture;
	}
}
