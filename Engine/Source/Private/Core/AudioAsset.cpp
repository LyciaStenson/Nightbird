#include "Core/AudioAsset.h"

#include "Core/Log.h"

NB_REFLECT_NO_FIELDS(Nightbird::Core::AudioAsset, NB_NO_PARENT, NB_NO_FACTORY)

namespace Nightbird::Core
{
	AudioAsset::AudioAsset(uint32_t sampleRate, uint32_t frameCount, uint8_t channels, AudioEncoding encoding, std::vector<std::vector<uint8_t>> channelData)
		: m_SampleRate(sampleRate), m_FrameCount(frameCount), m_Channels(channels), m_Encoding(encoding), m_ChannelData(std::move(channelData))
	{
		if (m_ChannelData.size() < m_Channels)
		{
			Core::Log::Warning("AudioAsset: m_ChannelData.size() < m_Channels");
			uint32_t bytesPerChannel = frameCount * sizeof(int16_t);
			for (size_t i = m_ChannelData.size(); i < m_Channels; ++i)
			{
				m_ChannelData.push_back(std::vector<uint8_t>(bytesPerChannel, 0));
			}
		}
	}

	uint32_t AudioAsset::GetSampleRate() const
	{
		return m_SampleRate;
	}

	uint32_t AudioAsset::GetFrameCount() const
	{
		return m_FrameCount;
	}

	uint8_t AudioAsset::GetChannels() const
	{
		return m_Channels;
	}

	AudioEncoding AudioAsset::GetEncoding() const
	{
		return m_Encoding;
	}

	const std::vector<uint8_t>& AudioAsset::GetChannelData(uint8_t channel) const
	{
		return m_ChannelData[channel];
	}
}
