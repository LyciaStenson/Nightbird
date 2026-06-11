#include "Core/BackendFactory.h"

#include "WiiU/Platform.h"
#include "GX2/GX2Renderer.h"

namespace Nightbird::Core
{
	std::unique_ptr<Core::Platform> CreatePlatform()
	{
		return std::make_unique<WiiU::Platform>();
	}

	std::unique_ptr<Core::Renderer> CreateRenderer()
	{
		return std::make_unique<GX2::Renderer>();
	}
}
