#include "Core/Engine.h"

#include "Core/Platform.h"
#include "Core/Renderer.h"
#include "Core/Scene.h"
#include "Core/SceneObject.h"
#include "Core/AssetManager.h"
#include "Core/Log.h"

#include "Input/InputProvider.h"

#include <chrono>

extern volatile int nb_link_AudioSource;
extern volatile int nb_link_Skybox;

namespace Nightbird::Core
{
	Engine::Engine(Platform& platform, Renderer& renderer, AssetManager& assetManager)
		: m_Platform(platform), m_Renderer(renderer), m_AssetManager(assetManager)
	{
		m_Scene = std::make_unique<Scene>();
		
		if (m_Scene)
			m_Scene->SetEngine(this);

		static volatile int sink = 0;
		sink += nb_link_AudioSource;
		sink += nb_link_Skybox;
	}

	Engine::~Engine()
	{
		m_Renderer.Shutdown();
		m_Platform.Shutdown();
	}

	bool Engine::ShouldClose() const
	{
		return m_Platform.ShouldClose();
	}

	float Engine::Update()
	{
		static auto lastTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		m_DeltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
		lastTime = currentTime;

		m_Platform.Update();
		m_InputSystem.Update(m_Platform.GetInputProvider());
		m_Scene->Update(m_DeltaTime);

		return m_DeltaTime;
	}

	Platform& Engine::GetPlatform()
	{
		return m_Platform;
	}

	Renderer& Engine::GetRenderer()
	{
		return m_Renderer;
	}

	Input::System& Engine::GetInputSystem()
	{
		return m_InputSystem;
	}

	Audio::Provider& Engine::GetAudioProvider()
	{
		return m_Platform.GetAudioProvider();
	}

	Scene& Engine::GetScene()
	{
		return *m_Scene;
	}

	void Engine::SetScene(std::unique_ptr<Scene> scene)
	{
		m_Scene = std::move(scene);

		if (m_Scene)
		{
			m_Scene->ResolveAssets(m_AssetManager);
			m_Scene->SetEngine(this);
		}
	}

	AssetManager& Engine::GetAssetManager()
	{
		return m_AssetManager;
	}

	float Engine::GetDeltaTime() const
	{
		return m_DeltaTime;
	}
}
