#pragma once

#include "PICA/PICATexture.h"

#include <memory>

namespace Nightbird::Core
{
	class Texture;
	struct Material;
}

namespace Nightbird::PICA
{
	class Material
	{
	public:
		Material() = default;
		Material(const Core::Material& material, std::shared_ptr<Texture> baseColorTexture);

		Material(const Material&) = delete;
		Material& operator=(const Material&) = delete;
		Material(Material&&) = default;
		Material& operator=(Material&&) = default;

		std::shared_ptr<Texture> GetBaseColorTexture();

	private:
		std::shared_ptr<Texture> m_BaseColorTexture;
	};
}
