#pragma once

#include <uuid.h>

namespace Nightbird::Core
{
	struct ProjectInfo
	{
		uuids::uuid mainSceneUUID;
	};

	class ProjectLoader
	{
	public:
		ProjectInfo Load(const std::string& cookedDir);
	};
}
