#pragma once

#include "EditorSettings.h"
#include "ProjectSettings.h"

#include <string>

namespace Nightbird::Editor
{
	class SettingsManager
	{
	public:
		EditorSettings LoadEditorSettings();
		void SaveEditorSettings(const EditorSettings& settings);

		ProjectSettings LoadProjectSettings(const std::string& projectPath);
		void SaveProjectSettings(const ProjectSettings& settings, const std::string& projectPath);
	};
}
