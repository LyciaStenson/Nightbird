#pragma once

#include "Core/TypeInfo.h"
#include "Cook/Endianness.h"

#include <uuid.h>

#include <filesystem>
#include <unordered_map>
#include <vector>

namespace Nightbird::Core
{
	class SceneObject;
	class Camera;
}

namespace Nightbird::Editor
{
	class BinaryWriter;

	class BinarySceneWriter
	{
	public:
		void Write(Core::SceneObject* root, const uuids::uuid& sceneUUID,
			const std::filesystem::path& outputPath, Endianness endianness,
			Core::Camera* activeCamera = nullptr);

	private:
		std::unordered_map<const Core::SceneObject*, uuids::uuid> m_NodeUUIDs;

		void AssignNodeUUIDs(Core::SceneObject* object, bool isRoot = false);
		void WriteNode(Core::SceneObject* object, const uuids::uuid& parentUUID, BinaryWriter& writer);
		void WriteFields(void* object, const TypeInfo* type, BinaryWriter& writer);
		
		uuids::uuid GenerateUUID() const;
	};
}
