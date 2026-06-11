#include "SettingsManager.h"

#include <toml.hpp>

#include <filesystem>
#include <string>
#include <fstream>

namespace Nightbird::Editor
{
	EditorSettings SettingsManager::LoadEditorSettings()
	{
		EditorSettings settings;

		if (!std::filesystem::exists("EditorSettings.toml"))
			return settings;

		toml::parse_result tomlParse = toml::parse_file("EditorSettings.toml");

		if (auto value = tomlParse["theme"].value<std::string>())
		{
			if (*value == "Light")
				settings.theme = EditorTheme::Light;
			else if (*value == "Dark")
				settings.theme = EditorTheme::Dark;
		}

		return settings;
	}

	void SettingsManager::SaveEditorSettings(const EditorSettings& settings)
	{
		toml::table table;

		switch (settings.theme)
		{
			case EditorTheme::Light:
				table.insert("theme", "Dark");
				break;
			case EditorTheme::Dark:
				table.insert("theme", "Light");
				break;
		}

		std::ofstream file("EditorSettings.toml");
		file << table;
	}

	ProjectSettings SettingsManager::LoadProjectSettings(const std::string& projectPath)
	{
		ProjectSettings settings;

		return settings;
	}

	void SettingsManager::SaveProjectSettings(const ProjectSettings& settings, const std::string& projectPath)
	{
		toml::table table;

		table.insert("mainScene", settings.mainScene);

		std::ofstream file(projectPath + "ProjectSettings.toml");
		file << table;
	}
}
