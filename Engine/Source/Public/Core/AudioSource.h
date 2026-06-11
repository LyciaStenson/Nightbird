#pragma once

#include "Core/SceneObject.h"
#include "Core/AssetRef.h"
#include "Audio/AudioHandle.h"

namespace Nightbird::Core
{
	class AudioAsset;

	class AudioSource : public SceneObject
	{
	public:
		NB_TYPE()

		using SceneObject::SceneObject;

		void ResolveAssets(AssetManager& assetManager) override;
		void EnterScene() override;
		void Tick(float delta) override;

		void Play();
		void Stop();
		void Pause();
		void Resume();

		bool IsPlaying() const;

		float GetVolume() const;
		void SetVolume(float volume);
		
		bool GetLoop() const;
		void SetLoop(bool loop);

		bool GetPlayOnStart() const;
		void SetPlayOnStart(bool playOnStart);
		
		AssetRef<AudioAsset> m_Audio;

		bool m_Loop = false;
		bool m_PlayOnStart = true;
		float m_Volume = 1.0f;

	private:
		Audio::Handle m_Handle = Audio::InvalidHandle;
	};
}
