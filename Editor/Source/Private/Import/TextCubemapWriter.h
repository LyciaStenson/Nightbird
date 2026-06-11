#pragma once

#include <uuid.h>

#include <filesystem>

namespace Nightbird::Editor
{
	struct AssetInfo;

	class TextCubemapWriter
	{
	public:
		void Write(const AssetInfo& assetInfo, const std::filesystem::path& outputPath);
	};
}