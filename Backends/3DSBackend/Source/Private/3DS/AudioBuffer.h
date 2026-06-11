#pragma once

#include <3ds.h>

#include <vector>

namespace Nightbird::Core
{
	class AudioAsset;
}

namespace Nightbird::N3DS
{
	class AudioBuffer
	{
	public:
		AudioBuffer() = default;
		~AudioBuffer();

		AudioBuffer(const AudioBuffer&) = delete;
		AudioBuffer& operator=(const AudioBuffer&) = delete;
		AudioBuffer(AudioBuffer&& other) noexcept;
		AudioBuffer& operator=(AudioBuffer&& other) noexcept;

		bool Load(const Core::AudioAsset& audio);
		void Play(int startChannel, bool loop);
		void Stop(int startChannel);

		bool IsInitialized() const;
		uint8_t GetChannels() const;

	private:
		static constexpr uint32_t k_DSPHeaderSize = 96;

		struct ChannelBuffer
		{
			void* linearData = nullptr;
			size_t dataSize = 0;
			ndspWaveBuf waveBuf{};
			ndspAdpcmData adpcmData{};
			u16 coefs[16]{};
			uint32_t sampleRate = 0;
			uint32_t sampleCount = 0;
		};

		std::vector<ChannelBuffer> m_ChannelBuffers;

		uint8_t m_Channels = 0;
		bool m_Initialized = false;

		void Cleanup();
	};
}
