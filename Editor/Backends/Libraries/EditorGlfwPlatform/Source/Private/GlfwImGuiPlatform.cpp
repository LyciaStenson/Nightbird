#include "GlfwImGuiPlatform.h"

#include "Glfw/Platform.h"

#include <imgui_impl_glfw.h>

namespace Nightbird::Editor
{
	GlfwImGuiPlatform::GlfwImGuiPlatform(Glfw::Platform& platform)
		: m_Platform(platform)
	{

	}

	void GlfwImGuiPlatform::Initialize()
	{
		ImGui_ImplGlfw_InitForVulkan(m_Platform.GetWindow(), true);
	}

	void GlfwImGuiPlatform::Shutdown()
	{
		ImGui_ImplGlfw_Shutdown();
	}

	void GlfwImGuiPlatform::NewFrame()
	{
		ImGui_ImplGlfw_NewFrame();
	}
}
