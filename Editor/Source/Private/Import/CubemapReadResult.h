#pragma once

#include <uuid.h>

#include <memory>

namespace Nightbird::Core
{
	class Cubemap;
}

namespace Nightbird::Editor
{
	struct CubemapReadResult
	{
		uuids::uuid uuid;
		std::shared_ptr<Core::Cubemap> cubemap;
	};
}
