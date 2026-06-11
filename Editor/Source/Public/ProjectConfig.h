#pragma once

#include <filesystem>

namespace Nightbird::Editor
{
	struct ProjectConfig
	{
		std::string name;
		std::filesystem::path path;
	};

	ProjectConfig LoadProjectConfig(const std::filesystem::path& projectPath);

	void GenerateProjectFile(const ProjectConfig& projectConfig, const std::filesystem::path& templatePath, const std::filesystem::path& outputPath);
}
