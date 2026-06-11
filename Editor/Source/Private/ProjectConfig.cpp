#include "ProjectConfig.h"

#include "Core/Log.h"

#include <toml.hpp>

#include <fstream>
#include <sstream>

namespace Nightbird::Editor
{
	ProjectConfig LoadProjectConfig(const std::filesystem::path& projectPath)
	{
		ProjectConfig config;

		if (!std::filesystem::exists(projectPath))
		{
			Nightbird::Core::Log::Error("Project file does not exist: " + projectPath.string());
			return config;
		}

		auto parse = toml::parse_file(projectPath.string());
		if (!parse)
		{
			Nightbird::Core::Log::Error("Failed to parse .nproject: " + projectPath.string());
			return config;
		}

		toml::table table = parse.table();

		config.name = table["project"]["name"].value_or(std::string{});
		config.path = projectPath;

		return config;
	}

	void GenerateProjectFile(const ProjectConfig& projectConfig, const std::filesystem::path& templatePath, const std::filesystem::path& outputPath)
	{
		if (!std::filesystem::exists(templatePath))
		{
			Nightbird::Core::Log::Error("Template not found at: " + templatePath.string());
			return;
		}

		std::ifstream templateFile(templatePath);
		std::stringstream buffer;
		buffer << templateFile.rdbuf();
		std::string fileContent = buffer.str();

		size_t pos = 0;
		const std::string placeholder = "%PROJECT_NAME%";
		while ((pos = fileContent.find(placeholder, pos)) != std::string::npos)
		{
			fileContent.replace(pos, placeholder.length(), projectConfig.name);
			pos += projectConfig.name.size();
		}

		std::ofstream outFile(outputPath);
		outFile << fileContent;
	}
}
