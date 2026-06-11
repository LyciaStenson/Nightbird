#pragma once

#include "Windows/SettingsWindow.h"

#include "ProjectSettings.h"

namespace Nightbird::Editor
{
	class ProjectSettingsWindow : public SettingsWindow
	{
	public:
		NB_TYPE()

		ProjectSettingsWindow(ProjectSettings& settings);
	};
}
