#pragma once

#include "Audio/AudioProvider.h"
#include "Audio/AudioHandle.h"

#include "Core/AudioAsset.h"

#include <miniaudio.h>

#include <memory>

namespace Nightbird::Core
{
	class AudioAsset;
}

namespace Nightbird::Glfw
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
			ma_audio_buffer audioBuffer;
			ma_sound sound;
			bool audioBufferInitialized = false;
			bool soundInitialized = false;
			bool playOnce = false;
		};

		ma_engine m_AudioEngine;
		Audio::Handle m_NextHandle = 1;
		std::vector<std::unique_ptr<ActiveSound>> m_ActiveSounds;

		bool m_Initialized = false;

		Audio::Handle StartSound(const Core::AudioAsset& audio, bool loop, bool playOnce);
		ActiveSound* FindSound(Audio::Handle handle);
		const ActiveSound* FindSound(Audio::Handle handle) const;
		void DestroySound(ActiveSound& sound);
	};
}
