#pragma once

#include "Core/Reflection.h"

#include <vector>
#include <cstdint>

namespace Nightbird::Core
{
	enum class AudioEncoding : uint8_t
	{
		PCM16 = 0,
		DSP_ADPCM = 1
	};

	class AudioAsset
	{
	public:
		NB_TYPE_BASE()
		AudioAsset(uint32_t sampleRate, uint32_t frameCount, uint8_t channels, AudioEncoding encoding, std::vector<std::vector<uint8_t>> channelData);

		uint32_t GetSampleRate() const;
		uint32_t GetFrameCount() const;
		uint8_t GetChannels() const;
		AudioEncoding GetEncoding() const;
		const std::vector<uint8_t>& GetChannelData(uint8_t channel) const;

	private:
		uint32_t m_SampleRate;
		uint32_t m_FrameCount;
		uint32_t m_Channels;
		AudioEncoding m_Encoding;
		std::vector<std::vector<uint8_t>> m_ChannelData;
	};
}
