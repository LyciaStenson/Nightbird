#pragma once

#include "Core/Platform.h"

#include "WiiU/InputProvider.h"
#include "WiiU/AudioProvider.h"

namespace Nightbird::WiiU
{
	class Platform : public Core::Platform
	{
	public:
		Platform();

		void Initialize(int width, int height, const char* name) override;
		void Shutdown() override;
		void Update() override;
		void WaitEvents() override;
		void Close() override;
		bool GetShouldClose() const override;
		void GetFramebufferSize(int* width, int* height) const override;

		std::string GetCookedAssetsPath() const override;

		Input::Provider& GetInputProvider() override;
		Audio::Provider& GetAudioProvider() override;

	private:
		InputProvider m_InputProvider;
		AudioProvider m_AudioProvider;
	};
}
