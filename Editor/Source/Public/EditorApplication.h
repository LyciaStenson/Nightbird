#pragma once

#include "Core/Engine.h"
#include "Core/Platform.h"
#include "Core/Renderer.h"
#include "Import/ImportManager.h"

#include "SettingsManager.h"
#include "EditorSettings.h"
#include "ProjectSettings.h"

#include "EditorUIBackend.h"
#include "ImGuiPlatform.h"
#include "ImGuiRenderer.h"
#include "EditorUI.h"
#include "EditorContext.h"
#include "WindowManager.h"
#include "Cook/CookManager.h"

#include "ProjectConfig.h"

#include <memory>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace Nightbird::Editor
{
	class EditorApplication
	{
	public:
		EditorApplication() = default;
		~EditorApplication() = default;

		int Run(int argc, char** argv);

	private:
		void InitializeEngine();
		void InitializeEditor();
		int LoadProject();
		void UnloadProject();
		void InitializeSettings();
		void InitializeWindows();
		void InitializeEditorUI();
		void InitializeImportManager();
		void InitializeCookManager();
		void RunEditorLoop();
		void Update();
		void Render();
		void Shutdown();

		std::unique_ptr<Core::Platform> m_Platform;
		std::unique_ptr<Core::Renderer> m_Renderer;
		std::unique_ptr<ImportManager> m_ImportManager;
		std::unique_ptr<Core::Engine> m_Engine;
		std::unique_ptr<EditorUIBackend> m_EditorUIBackend;
		std::unique_ptr<EditorUI> m_EditorUI;
		std::unique_ptr<EditorContext> m_EditorContext;
		std::unique_ptr<WindowManager> m_WindowManager;
		std::unique_ptr<CookManager> m_CookManager;

		SettingsManager m_SettingsManager;
		EditorSettings m_EditorSettings;
		ProjectSettings m_ProjectSettings;

		ProjectConfig m_ProjectConfig;
		bool m_ProjectLoaded = false;

#ifdef _WIN32
		HMODULE m_ProjectLibHandle = nullptr;
#else
		void* m_ProjectLibHandle = nullptr;
#endif
	};
}
