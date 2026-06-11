#include "EditorUI.h"

#include "WindowManager.h"

#include "Windows/BuildWindow.h"
#include "Windows/EditorSettingsWindow.h"
#include "Windows/ProjectSettingsWindow.h"
#include "Windows/AboutWindow.h"

#include "Import/ImportManager.h"

#include "Scene/TextSceneWriter.h"

#include "EditorContext.h"

#include "Core/Engine.h"
#include "Core/Platform.h"

namespace Nightbird::Editor
{
	EditorUI::EditorUI(EditorContext& context, WindowManager& windowManager)
		: m_Context(context), m_WindowManager(windowManager)
	{

	}

	void EditorUI::ApplyTheme(EditorTheme theme)
	{
		ImGuiStyle& style = ImGui::GetStyle();

		switch (theme)
		{
			case EditorTheme::Light:
				ImGui::StyleColorsLight();
				break;
			case EditorTheme::Dark:
				ImGui::StyleColorsDark();

				ImVec4* colors = ImGui::GetStyle().Colors;
				colors[ImGuiCol_WindowBg]					= ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
				colors[ImGuiCol_Border]						= ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
				colors[ImGuiCol_BorderShadow]				= ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
				colors[ImGuiCol_TitleBg]					= ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
				colors[ImGuiCol_TitleBgActive]				= ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
				colors[ImGuiCol_TitleBgCollapsed]			= ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
				colors[ImGuiCol_Button]						= ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
				colors[ImGuiCol_ButtonHovered]				= ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
				colors[ImGuiCol_ButtonActive]				= ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
				colors[ImGuiCol_Header]						= colors[ImGuiCol_Button];
				colors[ImGuiCol_HeaderHovered]				= colors[ImGuiCol_ButtonHovered];
				colors[ImGuiCol_HeaderActive]				= colors[ImGuiCol_Button];
				colors[ImGuiCol_Separator]					= ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
				colors[ImGuiCol_SeparatorHovered]			= ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
				colors[ImGuiCol_SeparatorActive]			= ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
				colors[ImGuiCol_Tab]						= colors[ImGuiCol_Button];
				colors[ImGuiCol_TabHovered]					= colors[ImGuiCol_ButtonHovered];
				colors[ImGuiCol_TabSelected]				= colors[ImGuiCol_Button];
				colors[ImGuiCol_TabSelectedOverline]		= ImVec4(0.50f, 0.05f, 0.50f, 0.00f);
				colors[ImGuiCol_TabDimmed]					= colors[ImGuiCol_Button];
				colors[ImGuiCol_TabDimmedSelected]			= colors[ImGuiCol_Button];
				colors[ImGuiCol_TabDimmedSelectedOverline]	= ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
				colors[ImGuiCol_DragDropTarget]				= ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
				colors[ImGuiCol_DragDropTargetBg]			= ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

				break;
		}
		
		style.FramePadding = ImVec2(8.0f, 8.0f);
		style.WindowPadding = ImVec2(12.0f, 12.0f);
		style.FrameRounding = 3.0f;
		style.WindowRounding = 10.0f;
		style.FrameBorderSize = 0.0f;
		style.WindowBorderSize = 0.0f;
	}

	void EditorUI::Render()
	{
		MainMenuBar();

		ImGui::DockSpaceOverViewport();

		m_WindowManager.Render();
	}

	uuids::uuid GenerateUUID()
	{
		std::random_device randomDevice;

		auto seedData = std::array<int, std::mt19937::state_size>{};
		std::generate(std::begin(seedData), std::end(seedData), std::ref(randomDevice));
		std::seed_seq seq(std::begin(seedData), std::end(seedData));
		std::mt19937 generator(seq);
		uuids::uuid_random_generator gen{generator};

		return gen();
	}

	void EditorUI::MainMenuBar()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Save Scene"))
				{
					TextSceneWriter sceneWriter;
					sceneWriter.Write(m_Context.GetEngine().GetScene(), "TestScene", GenerateUUID(), m_Context.GetImportManager().GetAssetsDir() / "Scenes/TestScene.ntscene");
				}
				if (ImGui::MenuItem("Build"))
				{
					if (auto* window = m_WindowManager.GetWindow<BuildWindow>())
						window->SetOpen(true);
				}
				if (ImGui::MenuItem("Exit"))
				{

				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Editor Settings"))
				{
					if (auto* window = m_WindowManager.GetWindow<EditorSettingsWindow>())
						window->SetOpen(true);
				}

				if (ImGui::MenuItem("Project Settings"))
				{
					if (auto* window = m_WindowManager.GetWindow<ProjectSettingsWindow>())
						window->SetOpen(true);
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Window"))
			{
				for (auto& window : m_WindowManager.GetWindows())
				{
					bool open = window->IsOpen();
					if (ImGui::MenuItem(window->GetTitle().c_str(), nullptr, &open))
						window->SetOpen(open);
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem("About"))
				{
					if (auto* aboutWindow = m_WindowManager.GetWindow<AboutWindow>())
						aboutWindow->SetOpen(true);
				}
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}
}
