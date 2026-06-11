#pragma once

#include <string>

namespace Nightbird::Input
{
	class Provider;
}

namespace Nightbird::Audio
{
	class Provider;
}

namespace Nightbird::Core
{
	class Platform
	{
	public:
		virtual ~Platform() = default;

		virtual void Initialize() = 0;
		virtual void Shutdown() = 0;
		virtual void Update() = 0;
		virtual void WaitEvents() = 0;
		virtual bool ShouldClose() const = 0;
		virtual void GetFramebufferSize(int* width, int* height) const = 0;

		virtual std::string GetCookedAssetsPath() const = 0;

		virtual Input::Provider& GetInputProvider() = 0;
		virtual Audio::Provider& GetAudioProvider() = 0;
	};
}
