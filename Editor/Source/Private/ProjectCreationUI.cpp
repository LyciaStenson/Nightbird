#include "ProjectCreationUI.h"

#include "ProjectConfig.h"
#include "Core/Log.h"

#include <imgui.h>

#include <filesystem>
#include <fstream>

namespace Nightbird::Editor
{
	void ProjectCreationUI::ApplyTheme(EditorTheme theme)
	{
		ImGuiStyle& style = ImGui::GetStyle();

		switch (theme)
		{
			case EditorTheme::Light:
			{
				ImGui::StyleColorsLight();
				break;
			}
			case EditorTheme::Dark:
			{
				ImGui::StyleColorsDark();

				ImVec4* colors = ImGui::GetStyle().Colors;
				colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
				colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
				colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
				colors[ImGuiCol_FrameBg] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
				colors[ImGuiCol_FrameBgHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
				colors[ImGuiCol_FrameBgActive] = ImVec4(0.34f, 0.34f, 0.34f, 1.00f);
				colors[ImGuiCol_TitleBg] = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
				colors[ImGuiCol_TitleBgActive] = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
				colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
				colors[ImGuiCol_Button] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
				colors[ImGuiCol_ButtonHovered] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
				colors[ImGuiCol_ButtonActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
				colors[ImGuiCol_Header] = colors[ImGuiCol_Button];
				colors[ImGuiCol_HeaderHovered] = colors[ImGuiCol_ButtonHovered];
				colors[ImGuiCol_HeaderActive] = colors[ImGuiCol_Button];
				colors[ImGuiCol_Separator] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
				colors[ImGuiCol_SeparatorHovered] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
				colors[ImGuiCol_SeparatorActive] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
				colors[ImGuiCol_Tab] = colors[ImGuiCol_Button];
				colors[ImGuiCol_TabHovered] = colors[ImGuiCol_ButtonHovered];
				colors[ImGuiCol_TabSelected] = colors[ImGuiCol_Button];
				colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.50f, 0.05f, 0.50f, 0.00f);
				colors[ImGuiCol_TabDimmed] = colors[ImGuiCol_Button];
				colors[ImGuiCol_TabDimmedSelected] = colors[ImGuiCol_Button];
				colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
				colors[ImGuiCol_DragDropTarget] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
				colors[ImGuiCol_DragDropTargetBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

				break;
			}
		}

		style.FramePadding = ImVec2(8.0f, 8.0f);
		style.WindowPadding = ImVec2(12.0f, 12.0f);
		style.FrameRounding = 3.0f;
		style.WindowRounding = 10.0f;
		style.FrameBorderSize = 0.0f;
		style.WindowBorderSize = 0.0f;
	}

	void ProjectCreationUI::Render()
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();

		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus;

		ImGui::Begin("##ProjectCreation", nullptr, flags);
		ImGui::Text("Project Creation");

		ImGui::Dummy(ImVec2(0.0f, 15.0f));

		ImGui::Text("Project Template:");
		ImGui::Dummy(ImVec2(0.0f, 2.0f));

		bool projectJustCreated = false;
		static bool projectCreated = false;
		if (projectCreated)
			ImGui::BeginDisabled();

		const char* projectTemplates[] = { "Blank", "First Person", "Third Person" };
		static int selectedTemplate = -1;

		ImVec2 templateSize = ImVec2(110.0f, 120.0f);

		for (int i = 0; i < IM_ARRAYSIZE(projectTemplates); ++i)
		{
			bool isSelected = (selectedTemplate == i);

			if (isSelected)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.29f, 0.29f, 0.72f, 1.00f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.16f, 0.16f, 0.40f, 1.00f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.19f, 0.19f, 0.48f, 1.00f));
			}
			
			if (i > 0)
				ImGui::BeginDisabled();

			if (ImGui::Button(projectTemplates[i], templateSize))
				selectedTemplate = i;

			if (i > 0)
				ImGui::EndDisabled();
			
			if (isSelected)
				ImGui::PopStyleColor(3);

			if (i < IM_ARRAYSIZE(projectTemplates) - 1)
				ImGui::SameLine();
		}

		ImGui::Dummy(ImVec2(0.0f, 15.0f));

		static char projectLocation[256] = "";
		static char projectName[256] = "";

		if (ImGui::BeginTable("ProjectForm", 2))
		{
			ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 120.0f);
			ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::TextUnformatted("Project Location:");

			ImGui::TableSetColumnIndex(1);
			ImGui::SetNextItemWidth(-FLT_MIN);
			ImGui::InputText("##Project Location", projectLocation, IM_ARRAYSIZE(projectLocation));

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::TextUnformatted("Project Name:");

			ImGui::TableSetColumnIndex(1);
			ImGui::SetNextItemWidth(-FLT_MIN);
			ImGui::InputText("##Project Name", projectName, IM_ARRAYSIZE(projectName));

			ImGui::EndTable();
		}

		if (projectLocation[0] == '\0' || projectName[0] == '\0' || selectedTemplate < 0)
			ImGui::BeginDisabled();

		if (ImGui::Button("Create Project"))
		{
			std::filesystem::path projectPath = std::filesystem::path(projectLocation).make_preferred() / projectName;
			std::filesystem::create_directories(projectPath);

			const char* envPath = std::getenv("NIGHTBIRD_PATH");
			if (envPath)
			{
				std::filesystem::path templatePath = std::filesystem::path(envPath) / "Templates" / "Projects" / "Blank";

				std::filesystem::copy(templatePath, projectPath, std::filesystem::copy_options::recursive);
				
				for (const auto& entry : std::filesystem::recursive_directory_iterator(projectPath))
				{
					if (!entry.is_regular_file())
						continue;

					const auto& path = entry.path();

					std::string filename = path.filename().string();
					if (filename.find("%PROJECT_NAME%") == std::string::npos)
						continue;
					
					ReplaceAll("%PROJECT_NAME%", projectName, filename);
					
					std::filesystem::path newPath = path.parent_path() / filename;

					std::error_code ec;
					std::filesystem::rename(path, newPath, ec);

					if (ec)
					{
						Core::Log::Error("ProjectCreationUI: Rename failed: " + ec.message());
					}
				}

				for (const auto& entry : std::filesystem::recursive_directory_iterator(projectPath))
				{
					if (!entry.is_regular_file())
						continue;

					std::ifstream in(entry.path(), std::ios::binary);
					if (!in)
						continue;
					
					std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

					ReplaceAll("%PROJECT_NAME%", projectName, content);

					std::string projectNameUpper = projectName;
					for (char& c : projectNameUpper)
						c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
					ReplaceAll("%PROJECT_NAME_API%", projectNameUpper + "_API", content);

					std::ofstream out(entry.path(), std::ios::binary | std::ios::trunc);
					out.write(content.data(), content.size());
				}
				
				projectCreated = true;
				projectJustCreated = true;
			}
			else
			{
				Core::Log::Warning("ProjectCreationUI: Failed to find NIGHTBIRD_PATH environment variable.");
			}
		}

		if (projectLocation[0] == '\0' || projectName[0] == '\0' || selectedTemplate < 0)
			ImGui::EndDisabled();

		// projectJustCreated guards against ending disabled region that was never opened
		if (projectCreated && !projectJustCreated)
			ImGui::EndDisabled();

		ImGui::End();
	}

	void ProjectCreationUI::ReplaceAll(const std::string& placeholder, const std::string& replace, std::string& content)
	{
		size_t pos = 0;
		while ((pos = content.find(placeholder, pos)) != std::string::npos)
		{
			content.replace(pos, placeholder.length(), replace);
			pos += replace.length();
		}
	}
}
