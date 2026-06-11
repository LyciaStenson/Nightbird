#pragma once

#include "EditorTheme.h"

namespace Nightbird::Editor
{
	class EditorContext;
	class WindowManager;

	class EditorUI
	{
	public:
		EditorUI(EditorContext& context, WindowManager& windowManager);

		void ApplyTheme(EditorTheme theme);

		void Render();

	private:
		void MainMenuBar();
		
		EditorContext& m_Context;
		WindowManager& m_WindowManager;
	};
}
