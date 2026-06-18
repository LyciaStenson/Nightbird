#pragma once

#include "ViewWindow.h"

#include <memory>

namespace Nightbird::Core
{
	class Camera;
}

namespace Nightbird::Editor
{
	class EditorContext;
	
	class AppViewWindow : public ViewWindow
	{
	public:
		NB_TYPE()
		AppViewWindow(EditorContext& context, bool open = true);

	protected:
		Core::Camera* GetCamera() override;
	};
}
