#include "Windows/EditorSettingsWindow.h"

NB_REFLECT_NO_FIELDS(Nightbird::Editor::EditorSettingsWindow, NB_PARENT(Nightbird::Editor::ImGuiWindow), NB_NO_FACTORY)

namespace Nightbird::Editor
{
	EditorSettingsWindow::EditorSettingsWindow(EditorSettings& settings)
		: SettingsWindow("Editor Settings")
	{
		//SetSettings(settings);
	}
}
