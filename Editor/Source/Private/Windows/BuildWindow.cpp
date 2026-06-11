#include "Windows/BuildWindow.h"

#include "EditorContext.h"

#include "Cook/CookManager.h"
#include "Import/ImportManager.h"
#include "Import/AssetInfo.h"

NB_REFLECT_NO_FIELDS(Nightbird::Editor::BuildWindow, NB_PARENT(Nightbird::Editor::ImGuiWindow), NB_NO_FACTORY)

namespace Nightbird::Editor
{
	BuildWindow::BuildWindow(EditorContext& context, bool open)
	: ImGuiWindow("Build Window", open, { ImGuiWindowFlags_NoDocking }), m_Context(context)
	{

	}

	void BuildWindow::OnRender()
	{
		static const char* s_PlatformNames[] = { "Desktop", "Wii U", "3DS" };
		static const CookTarget s_PlatformTargets[] = { CookTarget::Desktop, CookTarget::WiiU, CookTarget::N3DS };

		ImGui::Text("Platform");
		ImGui::SameLine();
		if (ImGui::BeginCombo("##Platform", s_PlatformNames[m_SelectedPlatform]))
		{
			for (int i = 0; i < 3; ++i)
			{
				bool selected = m_SelectedPlatform == i;
				if (ImGui::Selectable(s_PlatformNames[i], selected))
					m_SelectedPlatform = i;
				if (selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImGui::Text("Main Scene");
		ImGui::SameLine();

		const char* previewName = m_SelectedSceneUUID.is_nil() ? "Select scene..." : m_SelectedSceneName.c_str();

		if (ImGui::BeginCombo("##MainScene", previewName))
		{
			auto& importManager = m_Context.GetImportManager();
			for (const auto& [uuid, assetInfo] : importManager.GetAssetInfos())
			{
				if (!importManager.GetAssetInfo(uuid))
					continue;

				auto* importer = importManager.FindImporter(assetInfo.path);
				if (!importer || !importer->AsSceneImporter())
					continue;

				std::string displayName = assetInfo.path.stem().string();

				bool selected = m_SelectedSceneUUID == uuid;
				if (ImGui::Selectable(displayName.c_str(), selected))
				{
					m_SelectedSceneUUID = uuid;
					m_SelectedSceneName = displayName;
				}
				if (selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		bool canCook = !m_SelectedSceneUUID.is_nil();
		if (!canCook)
			ImGui::BeginDisabled();

		if (ImGui::Button("Cook"))
			m_Context.GetCookManager().CookScene(m_SelectedSceneUUID, s_PlatformTargets[m_SelectedPlatform]);

		if(!canCook)
			ImGui::EndDisabled();
	}
}
