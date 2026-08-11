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
		void RenderItemContextMenu(const std::filesystem::path& path);
		void RenderRenameItem(const std::filesystem::path& path);

		EditorContext& m_Context;

		std::filesystem::path m_RenamingPath;
		char m_RenameBuffer[256] = {};
		bool m_RenameFocusRequested = false;
	};
}
