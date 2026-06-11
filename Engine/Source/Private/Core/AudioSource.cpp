#include "Core/AudioSource.h"

#include "Audio/AudioProvider.h"
#include "Core/AudioAsset.h"
#include "Core/Engine.h"
#include "Core/Log.h"

NB_REFLECT(Nightbird::Core::AudioSource, NB_PARENT(Nightbird::Core::SceneObject), NB_FACTORY(Nightbird::Core::AudioSource),
	NB_FIELD(m_Audio), NB_FIELD(m_Loop), NB_FIELD(m_PlayOnStart), NB_FIELD(m_Volume)
)

volatile int nb_link_AudioSource = 0;

namespace Nightbird::Core
{
	void AudioSource::ResolveAssets(AssetManager& assetManager)
	{
		m_Audio.Resolve(assetManager.Load<AudioAsset>(m_Audio.GetUUID()));
	}

	void AudioSource::EnterScene()
	{
		if (m_PlayOnStart && m_Audio)
			Play();
	}

	void AudioSource::Tick(float delta)
	{

	}

	void AudioSource::Play()
	{
		Engine* engine = GetEngine();
		if (!engine)
		{
			Core::Log::Warning("AudioSource: Play called before added to scene");
			return;
		}
		
		if (!m_Audio)
		{
			Core::Log::Warning("AudioSource: Missing audio asset");
			return;
		}

		if (m_Handle != Audio::InvalidHandle)
			Stop();
		
		m_Handle = engine->GetAudioProvider().Play(*m_Audio.Get(), m_Loop);

		if (m_Handle == Audio::InvalidHandle)
		{
			Core::Log::Warning("AudioSource: Failed to play audio");
			return;
		}

		engine->GetAudioProvider().SetVolume(m_Handle, m_Volume);
	}

	void AudioSource::Stop()
	{
		if (m_Handle == Audio::InvalidHandle)
			return;

		Engine* engine = GetEngine();
		if (!engine)
			return;

		engine->GetAudioProvider().Stop(m_Handle);
		m_Handle = Audio::InvalidHandle;
	}

	void AudioSource::Pause()
	{
		if (m_Handle == Audio::InvalidHandle)
			return;

		Engine* engine = GetEngine();
		if (!engine)
			return;

		engine->GetAudioProvider().Pause(m_Handle);
	}

	void AudioSource::Resume()
	{
		if (m_Handle == Audio::InvalidHandle)
			return;

		Engine* engine = GetEngine();
		if (!engine)
			return;

		engine->GetAudioProvider().Resume(m_Handle);
	}

	bool AudioSource::IsPlaying() const
	{
		if (m_Handle == Audio::InvalidHandle)
			return false;

		Engine* engine = GetEngine();
		if (!engine)
			return false;

		return engine->GetAudioProvider().IsPlaying(m_Handle);
	}

	float AudioSource::GetVolume() const
	{
		return m_Volume;
	}

	void AudioSource::SetVolume(float volume)
	{
		if (m_Handle == Audio::InvalidHandle)
			return;

		Engine* engine = GetEngine();
		if (!engine)
			return;

		engine->GetAudioProvider().SetVolume(m_Handle, volume);
	}
	
	bool AudioSource::GetLoop() const
	{
		return m_Loop;
	}

	void AudioSource::SetLoop(bool loop)
	{
		m_Loop = loop;
	}

	bool AudioSource::GetPlayOnStart() const
	{
		return m_PlayOnStart;
	}

	void AudioSource::SetPlayOnStart(bool playOnStart)
	{
		m_PlayOnStart = playOnStart;
	}
}
