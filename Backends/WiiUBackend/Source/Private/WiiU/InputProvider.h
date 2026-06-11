#pragma once

#include "Input/InputProvider.h"
#include "Input/InputState.h"

#include <vpad/input.h>

namespace Nightbird::WiiU
{
	class InputProvider : public Input::Provider
	{
	public:
		InputProvider() = default;
		~InputProvider() = default;

		void Poll(Input::State& state) override;

	private:
		void PollGamepad(Input::State& state);

		static Input::Digital ResolveVpadButton(uint32_t vpadButton);
	};
}
