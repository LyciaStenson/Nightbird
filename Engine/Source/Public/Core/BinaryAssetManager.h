#pragma once

#include "Core/AssetManager.h"

#include "Core/SceneReadResult.h"
#include "Core/BinarySceneReader.h"
#include "Core/ProjectLoader.h"
#include "Core/MeshLoader.h"
#include "Core/MaterialLoader.h"
#include "Core/TextureLoader.h"
#include "Core/AudioLoader.h"

#include <uuid.h>

#include <string>
#include <memory>
#include <unordered_map>

namespace Nightbird::Core
{
	class SceneObject;
	class Mesh;
	class AudioAsset;
	
	struct ProjectInfo;

	class BinaryAssetManager : public AssetManager
	{
	public:
		explicit BinaryAssetManager(const std::string& cookedDir);
		
		ProjectInfo LoadProject();

		SceneReadResult LoadScene(const uuids::uuid& uuid) override;
		
	protected:
		std::shared_ptr<Mesh> LoadMesh(const uuids::uuid& uuid) override;
		std::shared_ptr<Material> LoadMaterial(const uuids::uuid& uuid) override;
		std::shared_ptr<Texture> LoadTexture(const uuids::uuid& uuid) override;
		std::shared_ptr<Cubemap> LoadCubemap(const uuids::uuid& uuid) override;
		std::shared_ptr<AudioAsset> LoadAudio(const uuids::uuid& uuid) override;

	private:
		std::string m_CookedDir;

		std::unique_ptr<BinarySceneReader> m_SceneReader;
		std::unique_ptr<ProjectLoader> m_ProjectLoader;
		std::unique_ptr<TextureLoader> m_TextureLoader;
		std::unique_ptr<MaterialLoader> m_MaterialLoader;
		std::unique_ptr<MeshLoader> m_MeshLoader;
		std::unique_ptr<AudioLoader> m_AudioLoader;
		
		void LoadNestedScenes(SceneObject* object);
	};
}
