#pragma once

#include "ImGuiWindow.h"

namespace Nightbird::Editor
{
	class AboutWindow : public ImGuiWindow
	{
	public:
		NB_TYPE()

		AboutWindow(bool open = false);

	protected:
		void OnRender() override;
	};
}
