#include "Application.h"

#include "Core/BackendFactory.h"
#include "Core/ProjectLoader.h"
#include "Core/Scene.h"
#include "Core/Log.h"

namespace Nightbird::App
{
	int Application::Run()
	{
		Initialize();
		int result = LoadProject();
		if (result != 0)
			return result;
		RunLoop();
		Shutdown();

		return 0;
	}

	void Application::Initialize()
	{
		m_Platform = Core::CreatePlatform();
		m_Renderer = Core::CreateRenderer();

		m_Platform->Initialize();
		m_Renderer->Initialize();
		m_Renderer->InitializeSurface(m_Renderer->GetDefaultSurface());

		Nightbird::TypeRegistry::InitReflection();

		m_AssetManager = std::make_unique<Core::BinaryAssetManager>(m_Platform->GetCookedAssetsPath());

		m_Engine = std::make_unique<Core::Engine>(*m_Platform, *m_Renderer, *m_AssetManager);
	}

	int Application::LoadProject()
	{
		Core::ProjectInfo project = m_AssetManager->LoadProject();

		Core::SceneReadResult result = m_AssetManager->LoadScene(project.mainSceneUUID);
		if (result.root)
		{
			auto scene = std::make_unique<Core::Scene>();
			scene->SetActiveCamera(result.activeCamera);

			for (auto& child : result.root->GetChildren())
				scene->GetRoot()->AddChild(std::move(child));

			m_Engine->SetScene(std::move(scene));
			return 0;
		}
		else
		{
			Core::Log::Error("Failed to load main scene");
			return -1;
		}
	}

	void Application::RunLoop()
	{
		while (!m_Engine->ShouldClose())
		{
			m_Engine->Update();

			int width, height;
			m_Platform->GetFramebufferSize(&width, &height);
			
			if (width == 0 || height == 0)
			{
				m_Platform->WaitEvents();
				continue;
			}

			auto& surface = m_Renderer->GetDefaultSurface();
			if (!m_Renderer->BeginFrame(surface))
				continue;
			if (m_Engine->GetScene().GetActiveCamera())
				m_Renderer->SubmitScene(m_Engine->GetScene(), *m_Engine->GetScene().GetActiveCamera());
			m_Renderer->DrawScene(surface);
			m_Renderer->EndFrame(surface);
		}
	}

	void Application::Shutdown()
	{

	}
}