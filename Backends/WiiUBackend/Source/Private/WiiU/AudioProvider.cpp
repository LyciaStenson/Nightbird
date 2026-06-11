#include "WiiU/AudioProvider.h"

#include "Core/Log.h"

#include <coreinit/memdefaultheap.h>
#include <coreinit/cache.h>
#include <coreinit/thread.h>

#include <cstring>
#include <algorithm>

namespace Nightbird::WiiU
{
	void AudioProvider::Initialize()
	{
		uint32_t oldAffinity = OSGetThreadAffinity(OSGetCurrentThread());
		OSSetThreadAffinity(OSGetCurrentThread(), OS_THREAD_ATTRIB_AFFINITY_CPU1);

		AXInitParams params{};
		params.renderer = AX_INIT_RENDERER_48KHZ;
		params.pipeline = AX_INIT_PIPELINE_SINGLE;
		AXInitWithParams(&params);

		OSSetThreadAffinity(OSGetCurrentThread(), oldAffinity);

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
		AXQuit();
		m_Initialized = false;
	}

	void AudioProvider::Update()
	{
		if (!m_Initialized)
			return;

		for (int i = static_cast<int>(m_ActiveSounds.size()) - 1; i >= 0; i--)
		{
			auto& sound = m_ActiveSounds[i];
			if (sound->playOnce)
			{
				if (!IsPlaying(sound->handle))
				{
					DestroySound(*sound);
					m_ActiveSounds.erase(m_ActiveSounds.begin() + i);
				}
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

		m_ActiveSounds.erase(
			std::remove_if(m_ActiveSounds.begin(), m_ActiveSounds.end(),
				[handle](const auto& s)
				{
					return s->handle == handle;
				}),
			m_ActiveSounds.end());
	}

	void AudioProvider::Pause(Audio::Handle handle)
	{
		if (!m_Initialized)
			return;

		ActiveSound* sound = FindSound(handle);
		if (!sound)
			return;

		for (AXVoice* voice : sound->voices)
		{
			if (voice)
			{
				AXVoiceBegin(voice);
				AXSetVoiceState(voice, AX_VOICE_STATE_STOPPED);
				AXVoiceEnd(voice);
			}
		}
	}

	void AudioProvider::Resume(Audio::Handle handle)
	{
		if (!m_Initialized)
			return;

		ActiveSound* sound = FindSound(handle);
		if (!sound)
			return;

		for (AXVoice* voice : sound->voices)
		{
			if (voice)
			{
				AXVoiceBegin(voice);
				AXSetVoiceState(voice, AX_VOICE_STATE_PLAYING);
				AXVoiceEnd(voice);
			}
		}
	}

	void AudioProvider::SetVolume(Audio::Handle handle, float volume)
	{
		if (!m_Initialized)
			return;

		// ActiveSound* sound = FindSound(handle);
		// if (!sound)
		// 	return;
  //
		// uint16_t axVolume = static_cast<uint16_t>(volume * 0x8000);
  //
		// for (AXVoice* voice : sound->voices)
		// {
		// 	if (!voice)
		// 		continue;
  //
		// 	AXVoiceBegin(voice);
  //
		// 	AXVoiceVeData veData{};
		// 	veData.volume = axVolume;
		// 	veData.delta = 0;
		// 	AXSetVoiceVe(voice, &veData);
  //
		// 	AXVoiceDeviceMixData tvMix{};
		// 	tvMix.bus[0].volume = axVolume;
		// 	tvMix.bus[1].volume = axVolume;
		// 	AXSetVoiceDeviceMix(voice, AX_DEVICE_TYPE_TV, 0, &tvMix);
  //
		// 	AXVoiceDeviceMixData drcMix{};
		// 	drcMix.bus[0].volume = axVolume;
		// 	drcMix.bus[1].volume = axVolume;
		// 	AXSetVoiceDeviceMix(voice, AX_DEVICE_TYPE_DRC, 0, &drcMix);
  //
		// 	AXVoiceEnd(voice);
		// }
	}

	bool AudioProvider::IsPlaying(Audio::Handle handle) const
	{
		if (!m_Initialized)
			return false;

		const ActiveSound* sound = FindSound(handle);
		if (!sound)
			return false;

		for (AXVoice* voice : sound->voices)
			if (voice && AXIsVoiceRunning(voice))
				return true;

		return false;
	}

	Audio::Handle AudioProvider::StartSound(const Core::AudioAsset& audio, bool loop, bool playOnce)
	{
		if (!m_Initialized)
			return Audio::InvalidHandle;

		if (audio.GetEncoding() != Core::AudioEncoding::PCM16)
		{
			Core::Log::Error("AudioProvider: Only PCM16 supported");
			return Audio::InvalidHandle;
		}

		uint8_t channels = audio.GetChannels();
		if (channels == 0)
		{
			Core::Log::Error("AudioProvider: Audio has zero channels");
			return Audio::InvalidHandle;
		}

		uint32_t frames = audio.GetFrameCount();

		auto activeSound = std::make_unique<ActiveSound>();
		activeSound->handle = m_NextHandle++;
		activeSound->looping = loop;
		activeSound->playOnce = playOnce;
		activeSound->channels = channels;

		std::vector<uint8_t*> buffers(channels, nullptr);
		for (uint8_t channel = 0; channel < channels; ++channel)
		{
			const auto& channelData = audio.GetChannelData(channel);
			if (channelData.empty())
			{
				Core::Log::Error("AudioProvider: Channel " + std::to_string(channel) + " is empty");
				return Audio::InvalidHandle;
			}

			uint8_t* buffer = static_cast<uint8_t*>(MEMAllocFromDefaultHeapEx(channelData.size(), 64));
			if (!buffer)
			{
				Core::Log::Error("AudioProvider: Failed to allocate buffer for channel: " + std::to_string(channel));

				for (AXVoice* voice : activeSound->voices)
					AXFreeVoice(voice);
				for (auto* buffer : activeSound->buffers)
					MEMFreeToDefaultHeap(buffer);

				return Audio::InvalidHandle;
			}

			std::memcpy(buffer, channelData.data(), channelData.size());
			DCFlushRange(buffer, channelData.size());
			buffers[channel] = buffer;
			activeSound->buffers.push_back(buffer);
		}

		if (channels == 2)
		{
			AXVoice* voiceLeft = AXAcquireVoice(31, nullptr, nullptr);
			AXVoice* voiceRight = AXAcquireVoice(31, nullptr, nullptr);

			if (!voiceLeft || !voiceRight)
			{
				Core::Log::Error("AudioProvider: Failed to acquire stereo voices");

				if (voiceLeft)
					AXFreeVoice(voiceLeft);
				if (voiceRight)
					AXFreeVoice(voiceRight);
				for (AXVoice* voice : activeSound->voices)
					AXFreeVoice(voice);
				for (auto* buffer : activeSound->buffers)
					MEMFreeToDefaultHeap(buffer);

				return Audio::InvalidHandle;
			}

			activeSound->voices.push_back(voiceLeft);
			activeSound->voices.push_back(voiceRight);

			SetupStereoVoices(voiceLeft, voiceRight, buffers[0], buffers[1], frames, audio.GetSampleRate(), loop);
		}

		Audio::Handle handle = activeSound->handle;
		m_ActiveSounds.push_back(std::move(activeSound));
		return handle;
	}

	void AudioProvider::SetupStereoVoices(AXVoice* voiceLeft, AXVoice* voiceRight, void* dataLeft, void* dataRight, uint32_t frames, uint32_t sampleRate, bool loop)
	{
		AXVoiceBegin(voiceLeft);
		AXVoiceBegin(voiceRight);

		AXSetVoiceType(voiceLeft,  AX_VOICE_TYPE_UNKNOWN);
		AXSetVoiceType(voiceRight, AX_VOICE_TYPE_UNKNOWN);

		AXVoiceOffsets offsetsLeft{};
		offsetsLeft.dataType       = AX_VOICE_FORMAT_LPCM16;
		offsetsLeft.loopingEnabled = loop ? AX_VOICE_LOOP_ENABLED : AX_VOICE_LOOP_DISABLED;
		offsetsLeft.loopOffset     = 0;
		offsetsLeft.endOffset      = frames - 1;
		offsetsLeft.currentOffset  = 0;
		offsetsLeft.data           = dataLeft;
		AXSetVoiceOffsets(voiceLeft, &offsetsLeft);

		AXVoiceOffsets offsetsRight = offsetsLeft;
		offsetsRight.data = dataRight;
		AXSetVoiceOffsets(voiceRight, &offsetsRight);

		float ratio = static_cast<float>(sampleRate) / static_cast<float>(AXGetInputSamplesPerSec());
		AXSetVoiceSrcRatio(voiceLeft,  ratio);
		AXSetVoiceSrcRatio(voiceRight, ratio);
		AXSetVoiceSrcType(voiceLeft,  AX_VOICE_SRC_TYPE_LINEAR);
		AXSetVoiceSrcType(voiceRight, AX_VOICE_SRC_TYPE_LINEAR);

		AXVoiceVeData veData{};
		veData.volume = 0x8000;
		veData.delta  = 0;
		AXSetVoiceVe(voiceLeft,  &veData);
		AXSetVoiceVe(voiceRight, &veData);

		// TV supports surround sound = 6
		AXVoiceDeviceMixData tvMixLeft[6] = {};
		AXVoiceDeviceMixData tvMixRight[6] = {};
		tvMixLeft[0].bus[0].volume  = 0x8000; // Left
		tvMixRight[1].bus[0].volume = 0x8000; // Right

		AXSetVoiceDeviceMix(voiceLeft,  AX_DEVICE_TYPE_TV, 0, tvMixLeft);
		AXSetVoiceDeviceMix(voiceRight, AX_DEVICE_TYPE_TV, 0, tvMixRight);

		// Gamepad (DRC) stereo = 2
		AXVoiceDeviceMixData drcMixLeft[2] = {};
		AXVoiceDeviceMixData drcMixRight[2] = {};
		drcMixLeft[0].bus[0].volume  = 0x8000;
		drcMixRight[1].bus[0].volume = 0x8000;

		AXSetVoiceDeviceMix(voiceLeft,  AX_DEVICE_TYPE_DRC, 0, drcMixLeft);
		AXSetVoiceDeviceMix(voiceRight, AX_DEVICE_TYPE_DRC, 0, drcMixRight);

		AXSetVoiceState(voiceLeft,  AX_VOICE_STATE_PLAYING);
		AXSetVoiceState(voiceRight, AX_VOICE_STATE_PLAYING);

		AXVoiceEnd(voiceRight);
		AXVoiceEnd(voiceLeft);
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
		for (auto& sound : m_ActiveSounds)
		{
			if (sound->handle == handle)
				return sound.get();
		}
		return nullptr;
	}

	void AudioProvider::DestroySound(ActiveSound& sound)
	{
		for (AXVoice* voice : sound.voices)
		{
			if (!voice)
				continue;

			AXVoiceBegin(voice);
			AXSetVoiceState(voice, AX_VOICE_STATE_STOPPED);
			AXVoiceEnd(voice);

			AXFreeVoice(voice);
		}

		for (auto* buffer : sound.buffers)
		{
			MEMFreeToDefaultHeap(buffer);
		}

		sound.voices.clear();
		sound.buffers.clear();
	}
}
