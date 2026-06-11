#include "InputProvider.h"

namespace Nightbird::WiiU
{
	void InputProvider::Poll(Input::State& state)
	{
		state.pressed.fill(false);
		state.released.fill(false);

		PollGamepad(state);
	}

	void InputProvider::PollGamepad(Input::State& state)
	{
		VPADStatus vpadStatus;
		VPADReadError vpadError;

		VPADRead(VPAD_CHAN_0, &vpadStatus, 1, &vpadError);

		if (vpadError != VPAD_READ_SUCCESS)
			return;

		static const uint32_t vpadButtons[] =
		{
			VPAD_BUTTON_A, VPAD_BUTTON_B, VPAD_BUTTON_X, VPAD_BUTTON_Y,
			VPAD_BUTTON_L, VPAD_BUTTON_R, VPAD_BUTTON_ZL, VPAD_BUTTON_ZR,
			VPAD_BUTTON_PLUS, VPAD_BUTTON_MINUS, VPAD_BUTTON_HOME,
			VPAD_BUTTON_UP, VPAD_BUTTON_DOWN, VPAD_BUTTON_LEFT, VPAD_BUTTON_RIGHT
		};

		for (uint32_t vpadButton : vpadButtons)
		{
			Input::Digital button = ResolveVpadButton(vpadButton);
			bool isDown = vpadStatus.hold & vpadButton;
			bool wasDown = state.down[static_cast<size_t>(button)];

			state.down[static_cast<size_t>(button)] = isDown;

			if (isDown && !wasDown)
				state.pressed[static_cast<size_t>(button)] = true;
			else if (!isDown && wasDown)
				state.released[static_cast<size_t>(button)] = true;
		}

		state.axes2D[static_cast<size_t>(Input::Analog2D::Pad_LeftStick)] = { vpadStatus.leftStick.x, vpadStatus.leftStick.y };
		state.axes2D[static_cast<size_t>(Input::Analog2D::Pad_RightStick)] = { vpadStatus.rightStick.x, vpadStatus.rightStick.y };
	}

	Input::Digital InputProvider::ResolveVpadButton(uint32_t vpadButton)
	{
		switch (vpadButton)
		{
			case VPAD_BUTTON_A:
				return Input::Digital::Pad_A;
			case VPAD_BUTTON_B:
				return Input::Digital::Pad_B;
			case VPAD_BUTTON_X:
				return Input::Digital::Pad_X;
			case VPAD_BUTTON_Y:
				return Input::Digital::Pad_Y;
			case VPAD_BUTTON_L:
				return Input::Digital::Pad_L;
			case VPAD_BUTTON_R:
				return Input::Digital::Pad_R;
			case VPAD_BUTTON_ZL:
				return Input::Digital::Pad_ZL;
			case VPAD_BUTTON_ZR:
				return Input::Digital::Pad_ZR;
			case VPAD_BUTTON_PLUS:
				return Input::Digital::Pad_Plus;
			case VPAD_BUTTON_MINUS:
				return Input::Digital::Pad_Minus;
			case VPAD_BUTTON_HOME:
				return Input::Digital::Pad_Home;
			case VPAD_BUTTON_UP:
				return Input::Digital::Pad_Up;
			case VPAD_BUTTON_DOWN:
				return Input::Digital::Pad_Down;
			case VPAD_BUTTON_LEFT:
				return Input::Digital::Pad_Left;
			case VPAD_BUTTON_RIGHT:
				return Input::Digital::Pad_Right;
			default:
				return Input::Digital::COUNT;
		}
	}
}
