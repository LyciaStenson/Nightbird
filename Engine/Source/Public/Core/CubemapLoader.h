#pragma once

#include <uuid.h>

#include <memory>
#include <string>

namespace Nightbird::Core
{
	class Cubemap;

	class CubemapLoader
	{
	public:
		std::shared_ptr<Cubemap> Load(const std::string& cookedDir, const uuids::uuid& uuid);
	};
}
