#pragma once

#include "EditorTheme.h"

#include <string>

namespace Nightbird::Editor
{
	class ProjectCreationUI
	{
	public:
		void ApplyTheme(EditorTheme theme);

		void Render();

		void ReplaceAll(const std::string& placeholder, const std::string& replace, std::string& content);
	};
}
