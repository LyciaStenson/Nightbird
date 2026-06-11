#include "EditorApplication.h"

#include "Core/Scene.h"
#include "Core/BackendFactory.h"
#include "Core/SceneObject.h"
#include "Core/Log.h"

#include "EditorBackendFactory.h"
#include "SettingsManager.h"

#include "Scene/TextSceneReader.h"

#include "Windows/BuildWindow.h"
#include "Windows/SceneWindow.h"
#include "Windows/SceneOutliner.h"
#include "Windows/Inspector.h"
#include "Windows/AssetBrowser.h"
#include "Windows/EditorSettingsWindow.h"
#include "Windows/ProjectSettingsWindow.h"
#include "Windows/AboutWindow.h"

#include <cstdlib>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace Nightbird::Editor
{
	int EditorApplication::Run(int argc, char** argv)
	{
		if (argc < 2)
		{
			Core::Log::Error("Usage: Editor <project_path> [options]");
			return 1;
		}
		
		m_ProjectConfig = LoadProjectConfig(argv[1]);
		if (m_ProjectConfig.name.empty())
		{
			Core::Log::Error("Invalid project name in: " + m_ProjectConfig.path.string());
			return 1;
		}

		const char* envPath = std::getenv("NIGHTBIRD_PATH");
		if (!envPath)
			return 1;

		std::filesystem::path installPath = std::filesystem::path(envPath);

		for (int i = 2; i < argc; ++i)
		{
			std::string arg = argv[i];
			if (arg == "--generate")
			{
				GenerateProjectFile(m_ProjectConfig, installPath / "Templates" / "premake5.template.lua", m_ProjectConfig.path.parent_path() / "premake5.lua");
				GenerateProjectFile(m_ProjectConfig, installPath / "Templates" / "Makefile.template.wiiu", m_ProjectConfig.path.parent_path() / "Makefile.wiiu");
				GenerateProjectFile(m_ProjectConfig, installPath / "Templates" / "Makefile.template.3ds", m_ProjectConfig.path.parent_path() / "Makefile.3ds");
				GenerateProjectFile(m_ProjectConfig, installPath / "Templates" / "Build-wiiu.template.bat", m_ProjectConfig.path.parent_path() / "Build-wiiu.bat");
				GenerateProjectFile(m_ProjectConfig, installPath / "Templates" / "Build-3ds.template.bat", m_ProjectConfig.path.parent_path() / "Build-3ds.bat");

#ifdef _WIN32
				GenerateProjectFile(m_ProjectConfig, installPath / "Templates" / "Build-vs2022.template.bat", m_ProjectConfig.path.parent_path() / "Build-vs2022.bat");
#else
				GenerateProjectFile(m_ProjectConfig, installPath / "Templates" / "Build-linux.template.sh", m_ProjectConfig.path.parent_path() / "Build-linux.sh");
				GenerateProjectFile(m_ProjectConfig, installPath / "Templates" / "Run-linux.template.sh", m_ProjectConfig.path.parent_path() / "Run-linux.sh");
				GenerateProjectFile(m_ProjectConfig, installPath / "Templates" / "Build-wiiu.template.sh", m_ProjectConfig.path.parent_path() / "Build-wiiu.sh");
				GenerateProjectFile(m_ProjectConfig, installPath / "Templates" / "Build-3ds.template.sh", m_ProjectConfig.path.parent_path() / "Build-3ds.sh");
#endif
				
				Core::Log::Info("Generated project build files for " + m_ProjectConfig.name);
				return 0;
			}
		}
		
		InitializeEngine();

		int result = LoadProject();
		if (result != 0)
			return result;

		InitializeEditor();
		
		RunEditorLoop();
		Shutdown();

		return 0;
	}

	void EditorApplication::InitializeEngine()
	{
		Nightbird::TypeRegistry::InitReflection();

		m_Platform = Core::CreatePlatform();
		m_Renderer = Core::CreateRenderer();
		
		m_ImportManager = std::make_unique<ImportManager>(m_ProjectConfig.path.parent_path() / "Assets");

		m_Engine = std::make_unique<Core::Engine>(*m_Platform, *m_Renderer, *m_ImportManager);

		m_Platform->Initialize();
		m_Renderer->Initialize();
	}
	
	int EditorApplication::LoadProject()
	{
		if (m_ProjectConfig.path.empty())
		{
			Core::Log::Error("No project specified");
			return 1;
		}
		
		// EDITORDEBUG ONLY
		std::string configStr = "EditorDebug";

		std::string platformStr;
#ifdef _WIN32
		platformStr = "windows-x86_64";
#else
		platformStr = "linux-x86_64";
#endif

		std::string libraryStr;
#ifdef _WIN32
		libraryStr = m_ProjectConfig.name + ".dll";
#else
		libraryStr = "lib" + m_ProjectConfig.name + ".so";
#endif

		std::filesystem::path sharedLibPath = m_ProjectConfig.path.parent_path() / "Binaries" / platformStr / configStr / libraryStr;

#ifdef _WIN32
		m_ProjectLibHandle = LoadLibraryA(sharedLibPath.string().c_str());
		if (!m_ProjectLibHandle)
		{
			DWORD error = GetLastError();
			Core::Log::Error("Failed to load project: " + sharedLibPath.string());
			Core::Log::Error(std::to_string(error));
			return 1;
		}
#else
		m_ProjectLibHandle = dlopen(sharedLibPath.string().c_str(), RTLD_NOW | RTLD_GLOBAL);
		if (!m_ProjectLibHandle)
		{
			Core::Log::Error("Failed to load project: " + sharedLibPath.string());
			Core::Log::Error(dlerror());
			return 1;
		}
#endif
		Core::Log::Info("Loaded project: " + sharedLibPath.string());

#ifdef _WIN32
		auto initProjectFn = reinterpret_cast<ProjectInitFn>(GetProcAddress(m_ProjectLibHandle, "NB_InitProject"));
#else
		auto initProjectFn = reinterpret_cast<ProjectInitFn>(dlsym(m_ProjectLibHandle, "NB_InitProject"));
#endif

		if (!initProjectFn)
		{
			Core::Log::Error("Failed to find NB_InitProject in Project shared library");
			UnloadProject();
			return 1;
		}

		initProjectFn([](TypeInfo* type)
			{
				TypeRegistry::Register(type);
			}
		);

		return 0;
	}

	void EditorApplication::UnloadProject()
	{
		if (m_ProjectLibHandle)
		{
#ifdef _WIN32
			FreeLibrary(m_ProjectLibHandle);
#else
			dlclose(m_ProjectLibHandle);
#endif
			m_ProjectLibHandle = nullptr;
			m_ProjectLoaded = false;
		}
	}

	void EditorApplication::InitializeEditor()
	{
		m_EditorUIBackend = CreateEditorUIBackend(m_Engine->GetPlatform(), m_Engine->GetRenderer());
		m_EditorUIBackend->Initialize();

		InitializeImportManager();
		InitializeCookManager();

		m_EditorContext = std::make_unique<EditorContext>(*m_Engine, *m_EditorUIBackend, *m_ImportManager, *m_CookManager);

		InitializeSettings();
		InitializeWindows();
		InitializeEditorUI();

		m_Renderer->InitializeSurface(m_WindowManager->GetWindow<SceneWindow>()->GetSurface());
	}

	void EditorApplication::InitializeSettings()
	{
		m_EditorSettings = m_SettingsManager.LoadEditorSettings();
		if (m_ProjectLoaded)
			m_ProjectSettings = m_SettingsManager.LoadProjectSettings(m_ProjectConfig.path.string());
	}

	void EditorApplication::InitializeWindows()
	{
		m_WindowManager = std::make_unique<WindowManager>();

		m_WindowManager->AddWindow<BuildWindow>(*m_EditorContext);
		m_WindowManager->AddWindow<SceneWindow>(*m_EditorContext);
		m_WindowManager->AddWindow<SceneOutliner>(*m_EditorContext);
		m_WindowManager->AddWindow<Inspector>(*m_EditorContext);
		m_WindowManager->AddWindow<AssetBrowser>(*m_EditorContext);
		m_WindowManager->AddWindow<Editor::EditorSettingsWindow>(m_EditorSettings);
		if (m_ProjectLoaded)
			m_WindowManager->AddWindow<Editor::ProjectSettingsWindow>(m_ProjectSettings);
		m_WindowManager->AddWindow<Editor::AboutWindow>();
	}

	void EditorApplication::InitializeEditorUI()
	{
		m_EditorUI = std::make_unique<EditorUI>(*m_EditorContext, *m_WindowManager);
		m_EditorUI->ApplyTheme(EditorTheme::Dark);
	}

	void EditorApplication::InitializeImportManager()
	{
		m_ImportManager->Scan();
	}

	void EditorApplication::InitializeCookManager()
	{
		m_CookManager = std::make_unique<CookManager>("Cooked", *m_ImportManager);
	}

	void EditorApplication::RunEditorLoop()
	{
		while (!m_Engine->ShouldClose())
		{
			Update();

			int width, height;
			m_Platform->GetFramebufferSize(&width, &height);

			if (width == 0 || height == 0)
			{
				m_Platform->WaitEvents();
				continue;
			}

			Render();
		}
	}

	void EditorApplication::Update()
	{
		m_Engine->Update();
	}

	void EditorApplication::Render()
	{
		auto& surface = m_Engine->GetRenderer().GetDefaultSurface();
		if (!m_Engine->GetRenderer().BeginFrame(surface))
			return;

		auto* sceneWindow = m_WindowManager->GetWindow<Editor::SceneWindow>();
		if (sceneWindow && sceneWindow->IsOpen())
		{
			if (sceneWindow->NeedsResize())
				sceneWindow->Resize(sceneWindow->GetPendingWidth(), sceneWindow->GetPendingHeight());

			m_Engine->GetRenderer().SubmitScene(m_Engine->GetScene(), sceneWindow->GetCamera());

			m_Engine->GetRenderer().BeginFrame(sceneWindow->GetSurface());
			m_Engine->GetRenderer().DrawScene(sceneWindow->GetSurface());
			m_Engine->GetRenderer().EndFrame(sceneWindow->GetSurface());
		}

		m_EditorUIBackend->BeginFrame();
		m_EditorUI->Render();
		m_EditorUIBackend->EndFrame();

		m_Engine->GetRenderer().EndFrame(surface);
	}

	void EditorApplication::Shutdown()
	{
		m_EditorUIBackend->Shutdown();
	}
}
