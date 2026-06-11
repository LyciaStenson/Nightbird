#pragma once

#include "Windows/SettingsWindow.h"

#include "EditorSettings.h"

namespace Nightbird::Editor
{
	class EditorSettingsWindow : public SettingsWindow
	{
	public:
		NB_TYPE()

		EditorSettingsWindow(EditorSettings& settings);
	};
}
