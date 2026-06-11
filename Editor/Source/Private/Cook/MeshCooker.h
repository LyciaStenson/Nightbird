#pragma once

#include "Cook/Endianness.h"

#include <uuid.h>

#include <filesystem>
#include <unordered_map>

namespace Nightbird::Core
{
	class Mesh;
	struct Material;
}

namespace Nightbird::Editor
{
	class MeshCooker
	{
	public:
		void Cook(const Core::Mesh& mesh, const uuids::uuid& uuid,
			const std::filesystem::path& outputDir, Endianness endianness,
			const std::unordered_map<const Core::Material*, uuids::uuid>& materialUUIDs);
	};
}