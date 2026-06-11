#pragma once

#include "Input/InputProvider.h"
#include "Input/InputState.h"

namespace Nightbird::N3DS
{
	class InputProvider : public Input::Provider
	{
	public:
		void Poll(Input::State& state) override;
	};
}
