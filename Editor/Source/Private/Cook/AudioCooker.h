#pragma once

#include "Cook/Target.h"
#include "Cook/Endianness.h"

#include <uuid.h>

#include <filesystem>
#include <vector>

namespace Nightbird::Editor
{
	class AudioCooker
	{
	public:
		void Cook(const std::filesystem::path& assetPath, const uuids::uuid& uuid, const std::filesystem::path& outputDir, CookTarget target, Endianness endianness);

	private:
		std::vector<uint8_t> CookPCM16(const std::filesystem::path& assetPath, uint32_t& outSampleRate, uint32_t& outFrameCount, uint8_t& outChannels, Endianness endianness, bool planar);
		std::vector<uint8_t> CookDSPADPCM(const std::filesystem::path& assetPath, const uuids::uuid& uuid, uint32_t& outSampleRate, uint32_t& outFrameCount, uint8_t& outChannels);
	};
}
