#include "Glfw/Platform.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#ifdef _WIN32
#include <windows.h>
#include <filesystem>
#endif

namespace Nightbird::Glfw
{
#ifdef _WIN32
	std::filesystem::path GetExeDir()
	{
		wchar_t buffer[MAX_PATH];
		GetModuleFileName(nullptr, buffer, MAX_PATH);
		return std::filesystem::path(buffer).parent_path();
	}
#endif

	void Platform::Initialize(int width, int height, const char* name)
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		m_Window = glfwCreateWindow(width, height, name, nullptr, nullptr);

		std::filesystem::path iconPath = GetExeDir() / "Icon.png";
		GLFWimage icon;
		icon.pixels = stbi_load(iconPath.string().c_str(), &icon.width, &icon.height, 0, 4);
		if (icon.pixels)
		{
			glfwSetWindowIcon(m_Window, 1, &icon);
			stbi_image_free(icon.pixels);
		}

		m_InputProvider = std::make_unique<InputProvider>(m_Window);

		m_AudioProvider.Initialize();
	}

	void Platform::Shutdown()
	{
		m_AudioProvider.Shutdown();

		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	void Platform::Update()
	{
		m_AudioProvider.Update();

		glfwPollEvents();
	}

	void Platform::WaitEvents()
	{
		glfwWaitEvents();
	}

	bool Platform::ShouldClose() const
	{
		return glfwWindowShouldClose(m_Window);
	}

	void Platform::GetFramebufferSize(int* width, int* height) const
	{
		glfwGetFramebufferSize(m_Window, width, height);
	}

	std::string Platform::GetCookedAssetsPath() const
	{
		return "Cooked";
	}

	Input::Provider& Platform::GetInputProvider()
	{
		return *m_InputProvider;
	}

	Audio::Provider& Platform::GetAudioProvider()
	{
		return m_AudioProvider;
	}

	std::vector<const char*> Platform::GetRequiredExtensions() const
	{
		uint32_t count = 0;
		const char** extensions = glfwGetRequiredInstanceExtensions(&count);
		return std::vector<const char*>(extensions, extensions + count);
	}

	GLFWwindow* Platform::GetWindow() const
	{
		return m_Window;
	}
}
