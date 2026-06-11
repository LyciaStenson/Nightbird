#include "WiiU/Platform.h"

#include "Core/Log.h"

#include <whb/proc.h>
#include <whb/sdcard.h>
#include <whb/log.h>
#include <whb/log_cafe.h>

namespace Nightbird::WiiU
{
	Platform::Platform()
	{
		WHBProcInit();
		WHBLogCafeInit();

		if (!WHBMountSdCard())
			Core::Log::Error("Failed to mount SD card");

		Core::Log::SetInfoFunc([](const std::string& message) { WHBLogPrintf("[Info] %s", message.c_str()); });
		Core::Log::SetWarningFunc([](const std::string& message) { WHBLogPrintf("[Warning] %s", message.c_str()); });
		Core::Log::SetErrorFunc([](const std::string& message) { WHBLogPrintf("[Error] %s", message.c_str()); });
	}

	void Platform::Initialize()
	{
		m_AudioProvider.Initialize();
	}

	void Platform::Shutdown()
	{
		m_AudioProvider.Shutdown();

		WHBUnmountSdCard();
		WHBLogCafeDeinit();
		WHBProcShutdown();
	}

	void Platform::Update()
	{
		m_AudioProvider.Update();
	}

	void Platform::WaitEvents()
	{

	}

	bool Platform::ShouldClose() const
	{
		return !WHBProcIsRunning();
	}

	void Platform::GetFramebufferSize(int* width, int* height) const
	{
		*width = 1920;
		*height = 1080;
	}

	std::string Platform::GetCookedAssetsPath() const
	{
		return std::string(WHBGetSdCardMountPath()) + "/Nightbird/Cooked";
	}

	Input::Provider& Platform::GetInputProvider()
	{
		return m_InputProvider;
	}

	Audio::Provider& Platform::GetAudioProvider()
	{
		return m_AudioProvider;
	}
}
