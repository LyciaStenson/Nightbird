#include "Windows/ProjectSettingsWindow.h"

NB_REFLECT_NO_FIELDS(Nightbird::Editor::ProjectSettingsWindow, NB_PARENT(Nightbird::Editor::ImGuiWindow), NB_NO_FACTORY)

namespace Nightbird::Editor
{
	ProjectSettingsWindow::ProjectSettingsWindow(ProjectSettings& settings)
		: SettingsWindow("Project Settings")
	{
		//SetSettings(settings);
	}
}
