#include "3DS/InputProvider.h"

#include <3ds.h>

namespace Nightbird::N3DS
{
	void InputProvider::Poll(Input::State& state)
	{
		state.pressed.fill(false);
		state.released.fill(false);

		u32 kDown = hidKeysDown();
		u32 kHeld = hidKeysHeld();
		u32 kUp = hidKeysUp();

		auto processButton = [&](Input::Digital button, u32 key)
		{
			size_t idx = static_cast<size_t>(button);
			bool isDown = kHeld & key;
			state.down[idx] = isDown;
			if (kDown & key)
				state.pressed[idx] = true;
			if (kUp & key)
				state.released[idx] = true;
		};

		processButton(Input::Digital::Pad_A, KEY_A);
		processButton(Input::Digital::Pad_B, KEY_B);
		processButton(Input::Digital::Pad_X, KEY_X);
		processButton(Input::Digital::Pad_Y, KEY_Y);
		processButton(Input::Digital::Pad_L, KEY_L);
		processButton(Input::Digital::Pad_R, KEY_R);
		processButton(Input::Digital::Pad_Start, KEY_START);
		processButton(Input::Digital::Pad_Select, KEY_SELECT);
		processButton(Input::Digital::Pad_Up, KEY_DUP);
		processButton(Input::Digital::Pad_Down, KEY_DDOWN);
		processButton(Input::Digital::Pad_Left, KEY_DLEFT);
		processButton(Input::Digital::Pad_Right, KEY_DRIGHT);

		circlePosition circlePos;
		hidCircleRead(&circlePos);
		state.axes2D[static_cast<size_t>(Input::Analog2D::Pad_LeftStick)] = { circlePos.dx / 156.0f, circlePos.dy / 156.0f };
	}
}
