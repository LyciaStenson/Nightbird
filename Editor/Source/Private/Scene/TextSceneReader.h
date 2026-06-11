#pragma once

#include "Core/TypeInfo.h"
#include "Core/SceneReadResult.h"

#include <uuid.h>
#include <toml.hpp>

#include <memory>
#include <filesystem>

namespace Nightbird::Editor
{
	class TextSceneReader
	{
	public:
		Core::SceneReadResult Read(const std::filesystem::path& path);

	private:
		void ReadFields(void* object, const TypeInfo* type, const toml::table& table);
	};
}
