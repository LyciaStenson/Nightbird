#pragma once

#include "Core/Renderable.h"
#include "Core/DirectionalLight.h"
#include "Core/PointLight.h"
#include "Core/Skybox.h"

#include <memory>
#include <vector>

namespace Nightbird::Core
{
	class Scene;
	class MeshInstance;
	class SceneObject;
	class Camera;

	class Scene
	{
	public:
		Scene();

		void Update(float delta);

		Engine* GetEngine() const;
		void SetEngine(Engine* engine);

		SceneObject* GetRoot();

		Camera* GetActiveCamera() const;
		void SetActiveCamera(Camera* camera);

		void ResolveAssets(AssetManager& assetManager);

		std::vector<Renderable> CollectRenderables() const;
		std::vector<DirectionalLight*> CollectDirectionalLights() const;
		std::vector<PointLight*> CollectPointLights() const;
		const Skybox* FindSkybox() const;

	private:
		Engine* m_Engine = nullptr;

		std::unique_ptr<SceneObject> m_Root;

		Camera* m_ActiveCamera = nullptr;

		void ResolveAssetsRecursive(SceneObject* object, AssetManager& assetManager);

		void UpdateRecursive(SceneObject* object, float delta);

		void CollectRenderablesRecursive(SceneObject* object, std::vector<Renderable>& renderables) const;
		void CollectDirectionalLightsRecursive(SceneObject* object, std::vector<DirectionalLight*>& directionalLights) const;
		void CollectPointLightsRecursive(SceneObject* object, std::vector<PointLight*>& pointLights) const;
		const Skybox* FindSkyboxRecursive(const SceneObject* object) const;
	};
}
