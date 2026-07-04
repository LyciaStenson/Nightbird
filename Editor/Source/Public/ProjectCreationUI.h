#pragma once

#include "EditorTheme.h"

namespace Nightbird::Editor
{
	class ProjectCreationUI
	{
	public:
		void ApplyTheme(EditorTheme theme);

		void Render();
	};
}
