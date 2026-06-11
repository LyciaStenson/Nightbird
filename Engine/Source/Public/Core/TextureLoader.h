#pragma once

#include <uuid.h>

#include <memory>
#include <string>

namespace Nightbird::Core
{
	class Texture;

	class TextureLoader
	{
	public:
		std::shared_ptr<Texture> Load(const std::string& cookedDir, const uuids::uuid& uuid);
	};
}
