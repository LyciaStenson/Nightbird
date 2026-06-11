#include "3DS/AudioBuffer.h"

#include "Core/AudioAsset.h"
#include "Core/Log.h"

#include <cstring>

namespace Nightbird::N3DS
{
	static uint32_t ReadBE32(const uint8_t* p)
	{
		return (uint32_t(p[0]) << 24) | (uint32_t(p[1]) << 16) |
			   (uint32_t(p[2]) << 8)  |  uint32_t(p[3]);
	}

	static uint16_t ReadBE16(const uint8_t* p)
	{
		return (uint16_t(p[0]) << 8) | uint16_t(p[1]);
	}

	AudioBuffer::~AudioBuffer()
	{
		Cleanup();
	}

	AudioBuffer::AudioBuffer(AudioBuffer&& other) noexcept
		: m_ChannelBuffers(std::move(other.m_ChannelBuffers)), m_Channels(other.m_Channels), m_Initialized(other.m_Initialized)
	{
		for (auto& buffer : m_ChannelBuffers)
			buffer.waveBuf.adpcm_data = &buffer.adpcmData;

		other.m_Channels = 0;
		other.m_Initialized = false;
	}

	AudioBuffer& AudioBuffer::operator=(AudioBuffer&& other) noexcept
	{
		if (this != &other)
		{
			Cleanup();

			m_ChannelBuffers = std::move(other.m_ChannelBuffers);
			m_Channels = other.m_Channels;
			m_Initialized = other.m_Initialized;

			for (auto& buffer : m_ChannelBuffers)
				buffer.waveBuf.adpcm_data = &buffer.adpcmData;

			other.m_Channels = 0;
			other.m_Initialized = false;
		}
		return *this;
	}

	bool AudioBuffer::Load(const Core::AudioAsset& audio)
	{
		Cleanup();

		m_Channels = audio.GetChannels();
		m_ChannelBuffers.resize(m_Channels);

		for (uint8_t channel = 0; channel < m_Channels; channel++)
		{
			const std::vector<uint8_t>& blob = audio.GetChannelData(channel);
			if (blob.size() < k_DSPHeaderSize)
			{
				Core::Log::Error("N3DS::AudioBuffer: Channel blob too small for channel " + std::to_string(channel));
				Cleanup();
				return false;
			}

			const uint8_t* dspHeader = blob.data();
			const uint8_t* adpcmData = blob.data() + k_DSPHeaderSize;
			size_t adpcmSize = blob.size() - k_DSPHeaderSize;

			uint32_t sampleCount = ReadBE32(dspHeader + 0x00);
			uint32_t sampleRate = ReadBE32(dspHeader + 0x08);
			uint32_t loopFlag = ReadBE16(dspHeader + 0x0C);

			int16_t coefs[16];
			for (int i = 0; i < 16; i++)
				coefs[i] = static_cast<int16_t>(ReadBE16(dspHeader + 0x1C + i * 2));

			uint16_t predScale = ReadBE16(dspHeader + 0x3E);
			int16_t history0 = static_cast<int16_t>(ReadBE16(dspHeader + 0x40));
			int16_t history1 = static_cast<int16_t>(ReadBE16(dspHeader + 0x42));

			ChannelBuffer& buffer = m_ChannelBuffers[channel];
			buffer.sampleRate = sampleRate;
			buffer.sampleCount = sampleCount;
			buffer.dataSize = adpcmSize;
			buffer.linearData = linearAlloc(adpcmSize);

			if (!buffer.linearData)
			{
				Core::Log::Error("N3DS::AudioBuffer: linearAlloc failed for channel " + std::to_string(channel));
				Cleanup();
				return false;
			}

			memcpy(buffer.linearData, adpcmData, adpcmSize);
			DSP_FlushDataCache(buffer.linearData, adpcmSize);

			for (int i = 0; i < 16; i++)
				buffer.coefs[i] = static_cast<u16>(coefs[i]);

			buffer.adpcmData.index = predScale;
			buffer.adpcmData.history0 = history0;
			buffer.adpcmData.history1 = history1;

			memset(&buffer.waveBuf, 0, sizeof(buffer.waveBuf));
			buffer.waveBuf.data_vaddr = buffer.linearData;
			buffer.waveBuf.nsamples = sampleCount;
			buffer.waveBuf.adpcm_data = &buffer.adpcmData;
			buffer.waveBuf.looping = (loopFlag != 0);
		}

		m_Initialized = true;
		return true;
	}

	void AudioBuffer::Play(int startChannel, bool loop)
	{
		if (!m_Initialized)
			return;

		for (uint8_t channel = 0; channel < m_Channels; channel++)
		{
			int ndspChannel = startChannel + channel;
			ChannelBuffer& buffer = m_ChannelBuffers[channel];

			buffer.waveBuf.looping = loop;

			ndspChnSetInterp(ndspChannel, NDSP_INTERP_LINEAR);
			ndspChnSetRate(ndspChannel, static_cast<float>(buffer.sampleRate));
			ndspChnSetFormat(ndspChannel, NDSP_FORMAT_MONO_ADPCM);
			ndspChnSetAdpcmCoefs(ndspChannel, buffer.coefs);

			float mix[12] = {};
			if (m_Channels == 1)
			{
				mix[0] = 1.0f;
				mix[1] = 1.0f;
			}
			else
			{
				if (channel == 0)
					mix[0] = 1.0f; // Left
				else
					mix[1] = 1.0f; // Right
			}

			ndspChnSetMix(ndspChannel, mix);
			ndspChnWaveBufAdd(ndspChannel, &buffer.waveBuf);
		}
	}

	void AudioBuffer::Stop(int startChannel)
	{
		if (!m_Initialized)
			return;

		for (uint8_t channel = 0; channel < m_Channels; channel++)
			ndspChnWaveBufClear(startChannel + channel);
	}

	void AudioBuffer::Cleanup()
	{
		for (auto& buffer : m_ChannelBuffers)
		{
			if (buffer.linearData)
			{
				linearFree(buffer.linearData);
				buffer.linearData = nullptr;
			}
		}

		m_ChannelBuffers.clear();
		m_Channels = 0;
		m_Initialized = false;
	}
}
