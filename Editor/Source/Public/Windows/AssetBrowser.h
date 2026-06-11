#pragma once

#include "ImGuiWindow.h"

#include <filesystem>

namespace Nightbird::Editor
{
	class EditorContext;

	class AssetBrowser : public ImGuiWindow
	{
	public:
		NB_TYPE()
		
		AssetBrowser(EditorContext& context, bool open = true);

	protected:
		void OnRender() override;

	private:
		EditorContext& m_Context;

		std::filesystem::path m_CurrentPath;
	};
}