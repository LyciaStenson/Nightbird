#include "Core/Scene.h"

#include "Core/Engine.h"
#include "Core/SceneObject.h"
#include "Core/MeshInstance.h"
#include "Core/Log.h"

namespace Nightbird::Core
{
	Scene::Scene()
	{
		m_Root = std::make_unique<SceneObject>();
	}
	
	void Scene::Update(float delta)
	{
		UpdateRecursive(m_Root.get(), delta);
	}

	Engine* Scene::GetEngine() const
	{
		return m_Engine;
	}

	void Scene::SetEngine(Engine* engine)
	{
		m_Engine = engine;
		m_Root->SetScene(this);
		m_Root->EnterSceneRecursive();
	}

	SceneObject* Scene::GetRoot()
	{
		return m_Root.get();
	}

	Camera* Scene::GetActiveCamera() const
	{
		return m_ActiveCamera;
	}

	void Scene::SetActiveCamera(Camera* camera)
	{
		m_ActiveCamera = camera;
	}

	void Scene::ResolveAssets(AssetManager& assetManager)
	{
		ResolveAssetsRecursive(m_Root.get(), assetManager);
	}

	void Scene::ResolveAssetsRecursive(SceneObject* object, AssetManager& assetManager)
	{
		object->ResolveAssets(assetManager);

		for (auto& child : object->GetChildren())
			ResolveAssetsRecursive(child.get(), assetManager);
	}

	std::vector<Renderable> Scene::CollectRenderables() const
	{
		std::vector<Renderable> renderables;
		CollectRenderablesRecursive(m_Root.get(), renderables);
		return renderables;
	}

	std::vector<DirectionalLight*> Scene::CollectDirectionalLights() const
	{
		std::vector<DirectionalLight*> directionalLights;
		CollectDirectionalLightsRecursive(m_Root.get(), directionalLights);
		return directionalLights;
	}

	std::vector<PointLight*> Scene::CollectPointLights() const
	{
		std::vector<PointLight*> pointLights;
		CollectPointLightsRecursive(m_Root.get(), pointLights);
		return pointLights;
	}

	const Skybox* Scene::FindSkybox() const
	{
		return FindSkyboxRecursive(m_Root.get());
	}

	void Scene::UpdateRecursive(SceneObject* object, float delta)
	{
		if (!object)
			return;

		object->Tick(delta);
		for (const auto& child : object->GetChildren())
			UpdateRecursive(child.get(), delta);
	}

	void Scene::CollectRenderablesRecursive(SceneObject* object, std::vector<Renderable>& renderables) const
	{
		if (!object)
			return;
		
		if (auto* meshInstance = Cast<MeshInstance>(object))
		{
			const Mesh* mesh = meshInstance->m_Mesh.Get().get();
			if (mesh)
			{
				for (size_t i = 0; i < mesh->GetPrimitiveCount(); i++)
				{
					Renderable renderable;
					renderable.primitive = &mesh->GetPrimitives()[i];
					renderable.transform = meshInstance->GetWorldMatrix();
					renderables.push_back(renderable);
				}
			}
		}

		for (const auto& child : object->GetChildren())
			CollectRenderablesRecursive(child.get(), renderables);
	}

	void Scene::CollectDirectionalLightsRecursive(SceneObject* object, std::vector<DirectionalLight*>& directionalLights) const
	{
		if (!object)
			return;

		if (auto* directionalLight = Cast<DirectionalLight>(object))
			directionalLights.push_back(directionalLight);

		for (const auto& child : object->GetChildren())
			CollectDirectionalLightsRecursive(child.get(), directionalLights);
	}
	
	void Scene::CollectPointLightsRecursive(SceneObject* object, std::vector<PointLight*>& pointLights) const
	{
		if (!object)
			return;

		if (auto* pointLight = Cast<PointLight>(object))
			pointLights.push_back(pointLight);

		for (const auto& child : object->GetChildren())
			CollectPointLightsRecursive(child.get(), pointLights);
	}

	const Skybox* Scene::FindSkyboxRecursive(const SceneObject* object) const
	{
		if (!object)
			return nullptr;
		if (const auto* skybox = Cast<Skybox>(object))
			return skybox;
		for (const auto& child : object->GetChildren())
		{
			if (const auto* skybox = FindSkyboxRecursive(child.get()))
				return skybox;
		}
		return nullptr;
	}
}
