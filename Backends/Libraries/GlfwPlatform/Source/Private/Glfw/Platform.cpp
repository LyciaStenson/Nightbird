#include "Glfw/Platform.h"

namespace Nightbird::Glfw
{
	void Platform::Initialize()
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		m_Window = glfwCreateWindow(1280, 720, "Nightbird", nullptr, nullptr);

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
