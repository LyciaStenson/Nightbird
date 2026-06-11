#pragma once

#include "Audio/AudioHandle.h"

namespace Nightbird::Core
{
	class AudioAsset;
}

namespace Nightbird::Audio
{
	class Provider
	{
	public:
		virtual ~Provider() = default;
		virtual void Initialize() = 0;
		virtual void Shutdown() = 0;
		virtual void Update() = 0;

		virtual Handle Play(const Core::AudioAsset& audio, bool loop) = 0;
		virtual void PlayOnce(const Core::AudioAsset& audio) = 0;
		virtual void Stop(Handle handle) = 0;
		virtual void Pause(Handle handle) = 0;
		virtual void Resume(Handle handle) = 0;

		virtual void SetVolume(Handle handle, float volume) = 0;

		virtual bool IsPlaying(Handle handle) const = 0;
	};
}
