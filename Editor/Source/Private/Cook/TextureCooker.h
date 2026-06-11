#pragma once

#include "Cook/Target.h"
#include "Cook/Endianness.h"

#include <uuid.h>

#include <filesystem>
#include <vector>

namespace Nightbird::Core
{
	class Texture;
}

namespace Nightbird::Editor
{
	class TextureCooker
	{
	public:
		void Cook(const Core::Texture& texture, const uuids::uuid& uuid, const std::filesystem::path& outputDir, CookTarget target, Endianness endianness);

	private:
		std::vector<uint8_t> CookRGBA(const Core::Texture& texture);
		std::vector<uint8_t> CookT3X(const Core::Texture& texture, const uuids::uuid& uuid);
	};
}
