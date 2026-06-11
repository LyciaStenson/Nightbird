#pragma once

#include "Core/Reflection.h"

#include <optional>
#include <string>

#include <imgui.h>

namespace Nightbird::Editor
{
	struct ImGuiWindowConfig
	{
		ImGuiWindowFlags flags = 0;
		std::optional<ImVec2> size;
		std::optional<ImVec2> padding;
	};

	class ImGuiWindow
	{
	public:
		NB_TYPE_BASE()

		ImGuiWindow(const std::string& title, bool open = true, const ImGuiWindowConfig& config = {});
		virtual ~ImGuiWindow() = default;

		void Render();
		void SetOpen(bool open);
		bool IsOpen() const;
		const std::string& GetTitle() const;

	protected:
		virtual void OnRender() = 0;

	private:
		std::string m_Title;
		bool m_Open;
		ImGuiWindowConfig m_Config;
	};
}
