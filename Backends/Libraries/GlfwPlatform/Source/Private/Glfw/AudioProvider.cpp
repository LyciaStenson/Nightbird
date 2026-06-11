#include "Glfw/AudioProvider.h"

#include "Core/AudioAsset.h"
#include "Core/Log.h"

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include <cstring>

namespace Nightbird::Glfw
{
	void AudioProvider::Initialize()
	{
		ma_engine_config config = ma_engine_config_init();
		ma_result result = ma_engine_init(&config, &m_AudioEngine);
		if (result != MA_SUCCESS)
		{
			Core::Log::Error("AudioProvider: Failed to initialize miniaudio engine");
			return;
		}

		m_Initialized = true;
		Core::Log::Info("AudioProvider: Initialized");
	}

	void AudioProvider::Shutdown()
	{
		if (!m_Initialized)
			return;

		for (auto& sound : m_ActiveSounds)
			DestroySound(*sound);

		m_ActiveSounds.clear();
		ma_engine_uninit(&m_AudioEngine);
		m_Initialized = false;
	}

	void AudioProvider::Update()
	{
		if (!m_Initialized)
			return;

		for (int i = static_cast<int>(m_ActiveSounds.size()) - 1; i >= 0; i--)
		{
			auto& sound = m_ActiveSounds[i];
			if (sound->playOnce && !ma_sound_is_playing(&sound->sound))
			{
				DestroySound(*sound);
				m_ActiveSounds.erase(m_ActiveSounds.begin() + i);
			}
		}
	}

	Audio::Handle AudioProvider::Play(const Core::AudioAsset& audio, bool loop)
	{
		return StartSound(audio, loop, false);
	}

	void AudioProvider::PlayOnce(const Core::AudioAsset& audio)
	{
		StartSound(audio, false, true);
	}

	void AudioProvider::Stop(Audio::Handle handle)
	{
		if (!m_Initialized)
			return;

		ActiveSound* sound = FindSound(handle);
		if (!sound)
			return;

		DestroySound(*sound);
		for (auto it = m_ActiveSounds.begin(); it != m_ActiveSounds.end(); ++it)
		{
			if ((*it)->handle == handle)
			{
				m_ActiveSounds.erase(it);
				break;
			}
		}
	}

	void AudioProvider::Pause(Audio::Handle handle)
	{
		if (!m_Initialized)
			return;

		ActiveSound* sound = FindSound(handle);
		if (sound)
			ma_sound_stop(&sound->sound);
	}

	void AudioProvider::Resume(Audio::Handle handle)
	{
		if (!m_Initialized)
			return;

		ActiveSound* sound = FindSound(handle);
		if (sound)
			ma_sound_start(&sound->sound);
	}

	void AudioProvider::SetVolume(Audio::Handle handle, float volume)
	{
		if (!m_Initialized)
			return;

		ActiveSound* sound = FindSound(handle);
		if (sound)
			ma_sound_set_volume(&sound->sound, volume);
	}

	bool AudioProvider::IsPlaying(Audio::Handle handle) const
	{
		if (!m_Initialized)
			return false;

		const ActiveSound* sound = FindSound(handle);
		if (!sound)
			return false;

		return ma_sound_is_playing(&sound->sound);
	}

	Audio::Handle AudioProvider::StartSound(const Core::AudioAsset& audio, bool loop, bool playOnce)
	{
		if (!m_Initialized)
			return Audio::InvalidHandle;

		// Currently only support PCM16
		if (audio.GetEncoding() != Core::AudioEncoding::PCM16)
		{
			Core::Log::Error("AudioProvider: Unsupported encoding for desktop audio");
			return Audio::InvalidHandle;
		}

		auto activeSound = std::make_unique<ActiveSound>();
		activeSound->handle = m_NextHandle++;
		activeSound->playOnce = playOnce;

		const auto& blob = audio.GetChannelData(0);
		if (blob.empty())
		{
			Core::Log::Error("AudioProvider: Audio data empty");
			return Audio::InvalidHandle;
		}

		ma_audio_buffer_config bufferConfig = ma_audio_buffer_config_init(ma_format_s16, audio.GetChannels(), audio.GetFrameCount(), blob.data(), nullptr);
		bufferConfig.sampleRate = audio.GetSampleRate();

		ma_result result = ma_audio_buffer_init(&bufferConfig, &activeSound->audioBuffer);
		if (result != MA_SUCCESS)
		{
			Core::Log::Error("AudioProvider: Failed to init audio buffer");
			return Audio::InvalidHandle;
		}
		activeSound->audioBufferInitialized = true;

		result = ma_sound_init_from_data_source(&m_AudioEngine, &activeSound->audioBuffer, 0, nullptr, &activeSound->sound);
		if (result != MA_SUCCESS)
		{
			Core::Log::Error("AudioProvider: Failed to init sound");
			return Audio::InvalidHandle;
		}
		activeSound->soundInitialized = true;

		ma_sound_set_looping(&activeSound->sound, loop ? MA_TRUE : MA_FALSE);
		ma_sound_start(&activeSound->sound);

		Audio::Handle handle = activeSound->handle;
		m_ActiveSounds.push_back(std::move(activeSound));
		return handle;
	}

	AudioProvider::ActiveSound* AudioProvider::FindSound(Audio::Handle handle)
	{
		for (auto& sound : m_ActiveSounds)
		{
			if (sound->handle == handle)
				return sound.get();
		}
		return nullptr;
	}

	const AudioProvider::ActiveSound* AudioProvider::FindSound(Audio::Handle handle) const
	{
		for (const auto& sound : m_ActiveSounds)
		{
			if (sound->handle == handle)
				return sound.get();
		}
		return nullptr;
	}

	void AudioProvider::DestroySound(ActiveSound& sound)
	{
		if (sound.soundInitialized)
		{
			ma_sound_stop(&sound.sound);
			ma_sound_uninit(&sound.sound);
			sound.soundInitialized = false;
		}

		if (sound.audioBufferInitialized)
		{
			ma_audio_buffer_uninit(&sound.audioBuffer);
			sound.audioBufferInitialized = false;
		}
	}
}
