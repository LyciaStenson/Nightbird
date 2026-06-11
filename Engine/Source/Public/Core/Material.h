#pragma once

#include <glm/glm.hpp>

#include <memory>

namespace Nightbird::Core
{
	class Texture;

	struct Material
	{
		glm::vec4 baseColorFactor = glm::vec4(1.0f);
		float metallicFactor = 1.0f;
		float roughnessFactor = 1.0f;
		std::shared_ptr<Texture> baseColorTexture;
		std::shared_ptr<Texture> metallicRoughnessTexture;
		std::shared_ptr<Texture> normalTexture;
		bool transparencyEnabled = false;
		bool doubleSided = false;
	};
}