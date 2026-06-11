#pragma once

#include "Audio/AudioProvider.h"
#include "Audio/AudioHandle.h"

#include "Core/AudioAsset.h"

#include <sndcore2/core.h>
#include <sndcore2/voice.h>
#include <sndcore2/device.h>

#include <vector>
#include <memory>

namespace Nightbird::WiiU
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
		struct ActiveSound
		{
			Audio::Handle handle;
			std::vector<AXVoice*> voices;
			std::vector<uint8_t*> buffers;
			bool looping = false;
			bool playOnce = false;
			uint8_t channels = 0;
		};

		Audio::Handle m_NextHandle = 1;
		std::vector<std::unique_ptr<ActiveSound>> m_ActiveSounds;

		bool m_Initialized = false;

		Audio::Handle StartSound(const Core::AudioAsset& audio, bool loop, bool playOnce);
		void SetupStereoVoices(AXVoice* voiceLeft, AXVoice* voiceRight, void* dataLeft, void* dataRight, uint32_t frames, uint32_t sampleRate, bool loop);
		ActiveSound* FindSound(Audio::Handle handle);
		const ActiveSound* FindSound(Audio::Handle handle) const;
		void DestroySound(ActiveSound& sound);
	};
}
