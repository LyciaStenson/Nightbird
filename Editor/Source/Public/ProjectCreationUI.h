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

		void ReplaceAll(std::string& string, const std::string& from, const std::string& to);
	};
}
