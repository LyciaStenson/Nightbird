#include "Core/BackendFactory.h"

#include "3DS/Platform.h"
#include "PICA/PICARenderer.h"

namespace Nightbird::Core
{
	std::unique_ptr<Core::Platform> CreatePlatform()
	{
		return std::make_unique<N3DS::Platform>();
	}

	std::unique_ptr<Core::Renderer> CreateRenderer()
	{
		return std::make_unique<PICA::Renderer>();
	}
}
