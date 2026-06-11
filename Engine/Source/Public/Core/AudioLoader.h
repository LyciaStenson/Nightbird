#pragma once

#include <uuid.h>

#include <memory>
#include <string>

namespace Nightbird::Core
{
	class AudioAsset;

	class AudioLoader
	{
	public:
		std::shared_ptr<AudioAsset> Load(const std::string& cookedDir, const uuids::uuid& uuid);
	};
}
