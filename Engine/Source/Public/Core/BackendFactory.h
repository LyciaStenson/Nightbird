#pragma once

#include <memory>

namespace Nightbird::Core
{
	class Platform;
	class Renderer;

	std::unique_ptr<Platform> CreatePlatform();
	std::unique_ptr<Renderer> CreateRenderer();
}
