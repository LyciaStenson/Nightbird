#pragma once

#include <uuid.h>

#include <string>
#include <filesystem>
#include <unordered_map>

namespace Nightbird::Editor
{
	struct AssetInfo
	{
		uuids::uuid uuid;
		std::string importer;
		std::filesystem::path path;
		std::unordered_map<std::string, std::string> tags;
	};
}