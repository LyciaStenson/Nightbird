#include "Glfw/InputProvider.h"

namespace Nightbird::Glfw
{
	InputProvider::InputProvider(GLFWwindow* window)
		: m_Window(window)
	{
		glfwSetWindowUserPointer(window, this);

		glfwSetScrollCallback(
			window,
			[](GLFWwindow* window, double xoffset, double yoffset)
			{
				auto* self = static_cast<InputProvider*>(glfwGetWindowUserPointer(window));
				self->m_ScrollDelta += static_cast<float>(yoffset);
			}
		);
	}

	void InputProvider::Poll(Input::State& state)
	{
		state.pressed.fill(false);
		state.released.fill(false);

		PollKeyboard(state);
		PollMouse(state);
		PollGamepad(state);
	}

	void InputProvider::PollKeyboard(Input::State& state)
	{
		static const int glfwKeys[] = {
			GLFW_KEY_SPACE, GLFW_KEY_ENTER, GLFW_KEY_ESCAPE,
			GLFW_KEY_A, GLFW_KEY_B, GLFW_KEY_C, GLFW_KEY_D, GLFW_KEY_E,
			GLFW_KEY_F, GLFW_KEY_G, GLFW_KEY_H, GLFW_KEY_I, GLFW_KEY_J,
			GLFW_KEY_K, GLFW_KEY_L, GLFW_KEY_M, GLFW_KEY_N, GLFW_KEY_O,
			GLFW_KEY_P, GLFW_KEY_Q, GLFW_KEY_R, GLFW_KEY_S, GLFW_KEY_T,
			GLFW_KEY_U, GLFW_KEY_V, GLFW_KEY_W, GLFW_KEY_X, GLFW_KEY_Y,
			GLFW_KEY_Z
		};

		for (int glfwKey : glfwKeys)
		{
			Input::Digital key = ResolveGlfwKey(glfwKey);
			bool isDown = glfwGetKey(m_Window, glfwKey) == GLFW_PRESS;
			bool wasDown = state.down[static_cast<size_t>(key)];

			state.down[static_cast<size_t>(key)] = isDown;

			if (isDown && !wasDown)
				state.pressed[static_cast<size_t>(key)] = true;
			else if (!isDown && wasDown)
				state.released[static_cast<size_t>(key)] = true;
		}
	}
	
	void InputProvider::PollMouse(Input::State& state)
	{
		struct MouseButtonPair
		{
			int glfwButton;
			Input::Digital input;
		};

		static constexpr MouseButtonPair buttons[] =
		{
			{ GLFW_MOUSE_BUTTON_LEFT, Input::Digital::Mouse_Left },
			{ GLFW_MOUSE_BUTTON_RIGHT, Input::Digital::Mouse_Right },
			{ GLFW_MOUSE_BUTTON_MIDDLE, Input::Digital::Mouse_Middle }
		};

		for (const auto& [glfwButton, input] : buttons)
		{
			bool isDown = glfwGetMouseButton(m_Window, glfwButton) == GLFW_PRESS;

			size_t index = static_cast<size_t>(input);

			bool wasDown = state.down[index];

			state.down[index] = isDown;

			if (isDown && !wasDown)
				state.pressed[index] = true;
			else if (!isDown && wasDown)
				state.released[index] = true;
		}

		state.axes1D[static_cast<size_t>(Input::Analog1D::Mouse_Wheel)] = m_ScrollDelta;
		m_ScrollDelta = 0.0f;

		double x, y;
		glfwGetCursorPos(m_Window, &x, &y);

		state.axes2D[static_cast<size_t>(Input::Analog2D::Mouse_Move)] =
		{
			static_cast<float>(x), static_cast<float>(y)
		};
	}
	
	void InputProvider::PollGamepad(Input::State& state)
	{
		if (!glfwJoystickIsGamepad(GLFW_JOYSTICK_1))
			return;

		GLFWgamepadstate gamepadState;
		if (!glfwGetGamepadState(GLFW_JOYSTICK_1, &gamepadState))
			return;
		
		static const int glfwButtons[] = {
			GLFW_GAMEPAD_BUTTON_A, GLFW_GAMEPAD_BUTTON_B,
			GLFW_GAMEPAD_BUTTON_X, GLFW_GAMEPAD_BUTTON_Y,
			GLFW_GAMEPAD_BUTTON_LEFT_BUMPER, GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER,
			GLFW_GAMEPAD_BUTTON_LEFT_THUMB, GLFW_GAMEPAD_BUTTON_RIGHT_THUMB,
			GLFW_GAMEPAD_BUTTON_START, GLFW_GAMEPAD_BUTTON_BACK,
			GLFW_GAMEPAD_BUTTON_GUIDE,
			GLFW_GAMEPAD_BUTTON_DPAD_UP, GLFW_GAMEPAD_BUTTON_DPAD_DOWN,
			GLFW_GAMEPAD_BUTTON_DPAD_LEFT, GLFW_GAMEPAD_BUTTON_DPAD_RIGHT
		};

		for (int glfwButton : glfwButtons)
		{
			Input::Digital button = ResolveGlfwButton(glfwButton);
			bool isDown = gamepadState.buttons[glfwButton] == GLFW_PRESS;
			bool wasDown = state.down[static_cast<size_t>(button)];

			state.down[static_cast<size_t>(button)] = isDown;

			if (isDown && !wasDown)
				state.pressed[static_cast<size_t>(button)] = true;
			else if (!isDown && wasDown)
				state.released[static_cast<size_t>(button)] = true;
		}

		state.axes2D[static_cast<size_t>(Input::Analog2D::Pad_LeftStick)] = { gamepadState.axes[GLFW_GAMEPAD_AXIS_LEFT_X], gamepadState.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] };
		state.axes2D[static_cast<size_t>(Input::Analog2D::Pad_RightStick)] = { gamepadState.axes[GLFW_GAMEPAD_AXIS_RIGHT_X], gamepadState.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] };
	}

	Input::Digital InputProvider::ResolveGlfwKey(int glfwKey)
	{
		switch (glfwKey)
		{
			case GLFW_KEY_SPACE:
				return Input::Digital::Key_Space;
			case GLFW_KEY_ENTER:
				return Input::Digital::Key_Enter;
			case GLFW_KEY_ESCAPE:
				return Input::Digital::Key_Escape;
			case GLFW_KEY_A:
				return Input::Digital::Key_A;
			case GLFW_KEY_B:
				return Input::Digital::Key_B;
			case GLFW_KEY_C:
				return Input::Digital::Key_C;
			case GLFW_KEY_D:
				return Input::Digital::Key_D;
			case GLFW_KEY_E:
				return Input::Digital::Key_E;
			case GLFW_KEY_F:
				return Input::Digital::Key_F;
			case GLFW_KEY_G:
				return Input::Digital::Key_G;
			case GLFW_KEY_H:
				return Input::Digital::Key_H;
			case GLFW_KEY_I:
				return Input::Digital::Key_I;
			case GLFW_KEY_J:
				return Input::Digital::Key_J;
			case GLFW_KEY_K:
				return Input::Digital::Key_K;
			case GLFW_KEY_L:
				return Input::Digital::Key_L;
			case GLFW_KEY_M:
				return Input::Digital::Key_M;
			case GLFW_KEY_N:
				return Input::Digital::Key_N;
			case GLFW_KEY_O:
				return Input::Digital::Key_O;
			case GLFW_KEY_P:
				return Input::Digital::Key_P;
			case GLFW_KEY_Q:
				return Input::Digital::Key_Q;
			case GLFW_KEY_R:
				return Input::Digital::Key_R;
			case GLFW_KEY_S:
				return Input::Digital::Key_S;
			case GLFW_KEY_T:
				return Input::Digital::Key_T;
			case GLFW_KEY_U:
				return Input::Digital::Key_U;
			case GLFW_KEY_V:
				return Input::Digital::Key_V;
			case GLFW_KEY_W:
				return Input::Digital::Key_W;
			case GLFW_KEY_X:
				return Input::Digital::Key_X;
			case GLFW_KEY_Y:
				return Input::Digital::Key_Y;
			case GLFW_KEY_Z:
				return Input::Digital::Key_Z;
			default:
				return Input::Digital::COUNT;
		}
	}

	Input::Digital InputProvider::ResolveGlfwButton(int glfwButton)
	{
		switch (glfwButton)
		{
		case GLFW_GAMEPAD_BUTTON_A:
			return Input::Digital::Pad_A;
		case GLFW_GAMEPAD_BUTTON_B:
			return Input::Digital::Pad_B;
		case GLFW_GAMEPAD_BUTTON_X:
			return Input::Digital::Pad_X;
		case GLFW_GAMEPAD_BUTTON_Y:
			return Input::Digital::Pad_Y;
		case GLFW_GAMEPAD_BUTTON_LEFT_BUMPER:
			return Input::Digital::Pad_L;
		case GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER:
			return Input::Digital::Pad_R;
		case GLFW_GAMEPAD_BUTTON_LEFT_THUMB:
			return Input::Digital::Pad_ZL;
		case GLFW_GAMEPAD_BUTTON_RIGHT_THUMB:
			return Input::Digital::Pad_ZR;
		case GLFW_GAMEPAD_BUTTON_START:
			return Input::Digital::Pad_Start;
		case GLFW_GAMEPAD_BUTTON_BACK:
			return Input::Digital::Pad_Back;
		case GLFW_GAMEPAD_BUTTON_GUIDE:
			return Input::Digital::Pad_Home;
		case GLFW_GAMEPAD_BUTTON_DPAD_UP:
			return Input::Digital::Pad_Up;
		case GLFW_GAMEPAD_BUTTON_DPAD_DOWN:
			return Input::Digital::Pad_Down;
		case GLFW_GAMEPAD_BUTTON_DPAD_LEFT:
			return Input::Digital::Pad_Left;
		case GLFW_GAMEPAD_BUTTON_DPAD_RIGHT:
			return Input::Digital::Pad_Right;
		default:
			return Input::Digital::COUNT;
		}
	}
}