#pragma once

#include "Cook/Endianness.h"

#include <uuid.h>

#include <filesystem>

namespace Nightbird::Core
{
	struct Material;
	class Texture;
}

namespace Nightbird::Editor
{
	class MaterialCooker
	{
	public:
		void Cook(const Core::Material& material, const uuids::uuid& uuid, const std::filesystem::path& outputDir, Endianness endianness, const std::unordered_map<const Core::Texture*, uuids::uuid>& textureUUIDs);
	};
}