#pragma once

#include "Core/SceneReadResult.h"
#include "Core/SceneObject.h"

#include "Cook/Target.h"
#include "Cook/ProjectCooker.h"
#include "Cook/TextureCooker.h"
#include "Cook/MaterialCooker.h"
#include "Cook/MeshCooker.h"
#include "Cook/AudioCooker.h"

#include "Scene/TextSceneWriter.h"

#include <uuid.h>

#include <filesystem>
#include <unordered_map>
#include <unordered_set>

namespace Nightbird::Core
{
	class Camera;
	class AudioSource;
	class Mesh;
	struct Material;
	class Texture;
}

namespace Nightbird::Editor
{
	class ImportManager;

	class CookManager
	{
	public:
		CookManager(const std::filesystem::path& outputDir, ImportManager& importManager);

		void CookScene(const uuids::uuid& sceneUUID, CookTarget target);
		void CookScene(Core::SceneReadResult, CookTarget target);

	private:
		std::filesystem::path m_RootOutputDir;
		std::filesystem::path m_CookOutputDir;

		Endianness m_Endianness = Endianness::Little;

		ImportManager& m_ImportManager;
		
		ProjectCooker m_ProjectCooker;
		TextureCooker m_TextureCooker;
		MaterialCooker m_MaterialCooker;
		MeshCooker m_MeshCooker;
		AudioCooker m_AudioCooker;

		std::unordered_map<const Core::Texture*, uuids::uuid> m_TextureUUIDs;
		std::unordered_map<const Core::Material*, uuids::uuid> m_MaterialUUIDs;
		std::unordered_map<const Core::Mesh*, uuids::uuid> m_MeshUUIDs;

		std::unordered_map<uuids::uuid, std::filesystem::path> m_AudioPathUUIDs;

		std::unordered_map<uuids::uuid, std::unique_ptr<Core::SceneObject>> m_ImportedSceneRoots;

		std::unordered_set<uuids::uuid> m_CookedSceneUUIDs;

		void WriteBinaryScene(Core::SceneObject* scene, const uuids::uuid& sceneUUID,
			const std::filesystem::path& outputDir, Endianness endianness, Core::Camera* activeCamera = nullptr);

		void CollectAssets(Core::SceneObject* object);

		void CookSceneInternal(Core::SceneReadResult&, CookTarget target);
		void CookTextures(const std::filesystem::path& outputDir, CookTarget target, Endianness endianness);
		void CookMaterials(const std::filesystem::path& outputDir, Endianness endianness);
		void CookMeshes(const std::filesystem::path& outputDir, Endianness endianness);
		void CookAudio(const std::filesystem::path& outputDir, CookTarget target, Endianness endianness);

		uuids::uuid GenerateUUID() const;
		Endianness GetEndianness(CookTarget target) const;
		std::filesystem::path GetOutputDir(CookTarget target) const;
	};
}
