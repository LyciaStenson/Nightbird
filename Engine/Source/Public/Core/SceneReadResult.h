#pragma once

#include <uuid.h>

#include <memory>

namespace Nightbird::Core
{
	class SceneObject;
	class Camera;

	struct SceneReadResult
	{
		std::unique_ptr<SceneObject> root;
		uuids::uuid uuid;
		Camera* activeCamera = nullptr;
	};
}
