#include "3DS/AudioProvider.h"

#include "Core/Log.h"

#include <uuid.h>

#include <cstring>

namespace Nightbird::N3DS
{
	void AudioProvider::Initialize()
	{
		Result rc = ndspInit();
		if (R_FAILED(rc))
		{
			Core::Log::Error("Failed to initialize NDSP. Check dspfirm.cdc is present.");
			return;
		}

		ndspSetOutputMode(NDSP_OUTPUT_STEREO);

		m_ActiveChannels.fill(false);

		m_Initialized = true;
		Core::Log::Info("Audio provider initialized");
	}

	void AudioProvider::Shutdown()
	{
		if (!m_Initialized)
			return;

		for (auto& sound : m_ActiveSounds)
			sound.buffer.Stop(sound.startChannel);

		m_ActiveSounds.clear();
		m_ActiveChannels.fill(false);

		ndspExit();
		m_Initialized = false;
	}

	void AudioProvider::Update()
	{
		if (!m_Initialized)
			return;

		for (int i = static_cast<int>(m_ActiveSounds.size()) - 1; i >= 0; i--)
		{
			ActiveSound& sound = m_ActiveSounds[i];
			if (!sound.looping)
			{
				bool done = true;
				for (uint8_t channel = 0; channel < sound.channelCount; channel++)
				{
					if (ndspChnIsPlaying(sound.startChannel + channel))
					{
						done = false;
						break;
					}
				}
				if (done)
				{
					FreeSound(sound);
					m_ActiveSounds.erase(m_ActiveSounds.begin() + i);
				}
			}
		}
	}

	Audio::Handle AudioProvider::Play(const Core::AudioAsset& audio, bool loop)
	{
		if (!m_Initialized)
			return Audio::InvalidHandle;

		uint8_t channelCount = audio.GetChannels();
		int startChannel = FindFreeChannels(channelCount);
		if (startChannel == -1)
		{
			Core::Log::Warning("AudioProvider: Play: No free channels available");
			return Audio::InvalidHandle;
		}

		ActiveSound sound;
		sound.handle = m_NextHandle++;
		sound.startChannel = startChannel;
		sound.channelCount = channelCount;
		sound.looping = loop;
		sound.playOnce = false;

		if (!sound.buffer.Load(audio))
		{
			Core::Log::Error("AudioProvider: Play: Failed to load audio buffer");
			return Audio::InvalidHandle;
		}

		for (uint8_t channel = 0; channel < channelCount; channel++)
			m_ActiveChannels[startChannel + channel] = true;

		sound.buffer.Play(startChannel, loop);

		Audio::Handle handle = sound.handle;
		m_ActiveSounds.push_back(std::move(sound));
		return handle;
	}

	void AudioProvider::PlayOnce(const Core::AudioAsset& audio)
	{
		if (!m_Initialized)
			return;

		uint8_t channelCount = audio.GetChannels();
		int startChannel = FindFreeChannels(channelCount);
		if (startChannel == -1)
		{
			Core::Log::Warning("AudioProvider: PlayOnce: No free channels available");
			return;
		}

		ActiveSound sound;
		sound.handle = m_NextHandle++;
		sound.startChannel = startChannel;
		sound.channelCount = channelCount;
		sound.looping = false;
		sound.playOnce = true;

		if (!sound.buffer.Load(audio))
		{
			Core::Log::Error("AudioProvider: PlayOnce: Failed to load audio buffer");
			return;
		}

		for (uint8_t channel = 0; channel < channelCount; channel++)
			m_ActiveChannels[startChannel + channel] = true;

		sound.buffer.Play(startChannel, false);
		m_ActiveSounds.push_back(std::move(sound));
	}

	void AudioProvider::Stop(Audio::Handle handle)
	{
		if (!m_Initialized)
			return;

		for (int i = static_cast<int>(m_ActiveSounds.size()) - 1; i >= 0; i--)
		{
			if (m_ActiveSounds[i].handle == handle)
			{
				FreeSound(m_ActiveSounds[i]);
				m_ActiveSounds.erase(m_ActiveSounds.begin() + i);
				return;
			}
		}
	}

	void AudioProvider::Pause(Audio::Handle handle)
	{
		if (!m_Initialized)
			return;

		for (auto& sound : m_ActiveSounds)
		{
			if (sound.handle == handle)
			{
				for (uint8_t channel = 0; channel < sound.channelCount; channel++)
					ndspChnSetPaused(sound.startChannel + channel, true);
				return;
			}
		}
	}

	void AudioProvider::Resume(Audio::Handle handle)
	{
		if (!m_Initialized)
			return;

		for (auto& sound : m_ActiveSounds)
		{
			if (sound.handle == handle)
			{
				for (uint8_t channel = 0; channel < sound.channelCount; channel++)
					ndspChnSetPaused(sound.startChannel + channel, false);
				return;
			}
		}
	}

	void AudioProvider::SetVolume(Audio::Handle handle, float volume)
	{
		if (!m_Initialized)
			return;

		for (auto& sound : m_ActiveSounds)
		{
			if (sound.handle == handle)
			{
				for (uint8_t channel = 0; channel < sound.channelCount; channel++)
				{
					float mix[12] = {};
					if (sound.channelCount == 1)
					{
						mix[0] = volume;
						mix[1] = volume;
					}
					else
					{
						if (channel == 0)
							mix[0] = volume; // Front left
						else
							mix[1] = volume; // Front right
					}
					ndspChnSetMix(sound.startChannel + channel, mix);
				}
				return;
			}
		}
	}

	bool AudioProvider::IsPlaying(Audio::Handle handle) const
	{
		for (const auto& sound : m_ActiveSounds)
		{
			if (sound.handle == handle)
			{
				for (uint8_t channel = 0; channel < sound.channelCount; channel++)
				{
					if (ndspChnIsPlaying(sound.startChannel + channel))
						return true;
				}
				return false;
			}
		}
		return false;
	}

	int AudioProvider::FindFreeChannels(uint8_t count) const
	{
		for (int i = 0; i <= k_MaxChannels - count; i++)
		{
			bool free = true;
			for (uint8_t j = 0; j < count; j++)
			{
				if (m_ActiveChannels[i + j])
				{
					free = false;
					break;
				}
			}
			if (free)
				return i;
		}
		return -1;
	}

	void AudioProvider::FreeSound(ActiveSound& sound)
	{
		sound.buffer.Stop(sound.startChannel);
		for (uint8_t channel = 0; channel < sound.channelCount; channel++)
			m_ActiveChannels[sound.startChannel + channel] = false;
	}
}
