#include "3DS/Platform.h"

#include "Core/Log.h"

#include <3ds.h>

#include <cstdio>

namespace Nightbird::N3DS
{
	Platform::Platform()
	{
		gfxInitDefault();
		consoleInit(GFX_BOTTOM, nullptr);

		Core::Log::SetInfoFunc([](const std::string& message) { printf("[Info] %s\n", message.c_str()); });
		Core::Log::SetWarningFunc([](const std::string& message) { printf("[Warning] %s\n", message.c_str()); });
		Core::Log::SetErrorFunc([](const std::string& message) { printf("[Error] %s\n", message.c_str()); });
	}

	void Platform::Initialize()
	{
		m_AudioProvider.Initialize();
	}

	void Platform::Shutdown()
	{
		m_AudioProvider.Shutdown();

		gfxExit();
	}

	void Platform::Update()
	{
		hidScanInput();

		m_AudioProvider.Update();
	}

	void Platform::WaitEvents()
	{

	}

	bool Platform::ShouldClose() const
	{
		return !aptMainLoop();
	}

	void Platform::GetFramebufferSize(int* width, int* height) const
	{
		*width = 400;
		*height = 240;
	}

	std::string Platform::GetCookedAssetsPath() const
	{
		return "sdmc:/Nightbird/Cooked";
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
