#include "ImGuiWindow.h"

#include <imgui_internal.h>

NB_REFLECT_NO_FIELDS(Nightbird::Editor::ImGuiWindow, NB_NO_PARENT, NB_NO_FACTORY)

namespace Nightbird::Editor
{
	static void DrawBorder(const std::string& title, float rounding, ImU32 color, float thickness, float padding)
	{
		::ImGuiWindow* window = ImGui::FindWindowByName(title.c_str());
		if (!window || window->Hidden || window->Collapsed)
			return;
		
		float inset = !window->DockIsActive ? thickness * 0.5f + padding : thickness * 0.5f;
		//float titleBarOffset = !window->DockIsActive ? window->TitleBarHeight : 0.0f;
		float titleBarOffset = window->TitleBarHeight;
		ImVec2 p_min = { window->Pos.x + inset,
						 window->Pos.y + titleBarOffset + inset };
		ImVec2 p_max = { window->Pos.x + window->Size.x - inset,
						 window->Pos.y + window->Size.y - inset };

		window->DrawList->AddRect(p_min, p_max, color, rounding, ImDrawFlags_RoundCornersAll, thickness);
	}

	ImGuiWindow::ImGuiWindow(const std::string& title, bool open, const ImGuiWindowConfig& config)
		: m_Title(title), m_Open(open), m_Config(config)
	{

	}
	
	void ImGuiWindow::Render()
	{
		if (!m_Open)
			return;

		if (m_Config.size)
			ImGui::SetNextWindowSize(*m_Config.size, ImGuiCond_Once);
		if (m_Config.padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, *m_Config.padding);

		if (ImGui::Begin(m_Title.c_str(), &m_Open, m_Config.flags))
			OnRender();
		ImGui::End();
		
		if (m_Config.padding)
			ImGui::PopStyleVar();

		DrawBorder(m_Title, 8.0f, IM_COL32(180, 180, 180, 255), 1.5f, 2.0f);
	}

	void ImGuiWindow::SetOpen(bool open)
	{
		m_Open = open;
	}

	bool ImGuiWindow::IsOpen() const
	{
		return m_Open;
	}

	const std::string& ImGuiWindow::GetTitle() const
	{
		return m_Title;
	}
}
