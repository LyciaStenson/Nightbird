#pragma once

#include "Audio/AudioProvider.h"
#include "Audio/AudioHandle.h"

#include "3DS/AudioBuffer.h"

#include "Core/AudioAsset.h"

#include <3ds.h>

#include <array>
#include <vector>

namespace Nightbird::N3DS
{
	class AudioProvider : public Audio::Provider
	{
	public:
		void Initialize() override;
		void Shutdown() override;
		void Update() override;

		Audio::Handle Play(const Core::AudioAsset& audio, bool loop) override;
		void PlayOnce(const Core::AudioAsset& audio) override;
		void Stop(Audio::Handle handle) override;
		void Pause(Audio::Handle handle) override;
		void Resume(Audio::Handle handle) override;

		void SetVolume(Audio::Handle handle, float volume) override;

		bool IsPlaying(Audio::Handle handle) const override;

	private:
		static constexpr int k_MaxChannels = 24;

		struct ActiveSound
		{
			Audio::Handle handle;
			AudioBuffer buffer;
			int startChannel;
			uint8_t channelCount;
			bool looping;
			bool playOnce;
		};

		std::array<bool, k_MaxChannels> m_ActiveChannels{};
		std::vector<ActiveSound> m_ActiveSounds;
		Audio::Handle m_NextHandle = 1;

		bool m_Initialized = false;

		int FindFreeChannels(uint8_t count) const;
		void FreeSound(ActiveSound& sound);
	};
}
