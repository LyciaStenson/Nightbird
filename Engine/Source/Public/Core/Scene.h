#pragma once

#include "Core/Renderable.h"
#include "Core/DirectionalLight.h"
#include "Core/PointLight.h"
#include "Core/AmbientLight.h"
#include "Core/Skybox.h"

#include <memory>
#include <vector>

namespace Nightbird::Core
{
	class Scene;
	class MeshInstance;
	class SceneObject;
	class Camera;

	class NB_API Scene
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
		std::vector<const DirectionalLight*> CollectDirectionalLights() const;
		std::vector<const PointLight*> CollectPointLights() const;
		const AmbientLight* FindAmbientLight() const;
		const Skybox* FindSkybox() const;

	private:
		Engine* m_Engine = nullptr;

		std::unique_ptr<SceneObject> m_Root;

		Camera* m_ActiveCamera = nullptr;

		void ResolveAssetsRecursive(SceneObject* object, AssetManager& assetManager);

		void UpdateRecursive(SceneObject* object, float delta);

		void CollectRenderablesRecursive(SceneObject* object, std::vector<Renderable>& renderables) const;
		void CollectDirectionalLightsRecursive(SceneObject* object, std::vector<const DirectionalLight*>& directionalLights) const;
		void CollectPointLightsRecursive(SceneObject* object, std::vector<const PointLight*>& pointLights) const;
		const AmbientLight* FindAmbientLightRecursive(const SceneObject* object) const;
		const Skybox* FindSkyboxRecursive(const SceneObject* object) const;
	};
}
