#pragma once

#include "Cook/Endianness.h"

#include <uuid.h>

#include <filesystem>

namespace Nightbird::Editor
{
	class ProjectCooker
	{
	public:
		void Cook(const uuids::uuid& mainSceneUUID, const std::filesystem::path& outputDir, Endianness Endianness);
	};
}
