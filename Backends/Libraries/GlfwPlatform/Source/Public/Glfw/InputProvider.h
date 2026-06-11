#pragma once

#include "Input/InputProvider.h"

#include <GLFW/glfw3.h>

namespace Nightbird::Glfw
{
	class InputProvider : public Input::Provider
	{
	public:
		InputProvider(GLFWwindow* window);
		~InputProvider() override = default;

		void Poll(Input::State& state) override;

	private:
		GLFWwindow* m_Window;

		float m_ScrollDelta = 0.0f;

		void PollKeyboard(Input::State& state);
		void PollMouse(Input::State& state);
		void PollGamepad(Input::State& state);

		static Input::Digital ResolveGlfwKey(int glfwKey);
		static Input::Digital ResolveGlfwButton(int glfwButton);
	};
}
