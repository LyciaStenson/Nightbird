#pragma once

#include "Core/TypeInfo.h"

#include <uuid.h>
#include <toml.hpp>

#include <string>
#include <filesystem>
#include <unordered_map>

namespace Nightbird::Core
{
	class Scene;
	class SceneObject;
}

namespace Nightbird::Editor
{
	class TextSceneWriter
	{
	public:
		void Write(Core::Scene& scene, const std::string& sceneName,
			const uuids::uuid& sceneUUID, const std::filesystem::path& outputPath);

	private:
		std::unordered_map<const Core::SceneObject*, uuids::uuid> m_NodeUUIDs;

		void AssignNodeUUIDs(Core::SceneObject* object);

		void WriteNode(Core::SceneObject* object, Core::SceneObject* parent, toml::array& nodesArray);
		void WriteFields(void* object, const TypeInfo* type, toml::table& table);

		uuids::uuid GenerateUUID() const;
	};
}
