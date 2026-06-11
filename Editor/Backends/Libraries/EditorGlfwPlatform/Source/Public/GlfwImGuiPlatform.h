#pragma once

#include "ImGuiPlatform.h"

struct GLFWwindow;

namespace Nightbird::Glfw
{
	class Platform;
}

namespace Nightbird::Editor
{
	class GlfwImGuiPlatform : public ImGuiPlatform
	{
	public:
		GlfwImGuiPlatform(Glfw::Platform& platform);

		void Initialize() override;
		void Shutdown() override;
		void NewFrame() override;

	private:
		Glfw::Platform& m_Platform;
	};
}
