#pragma once

#include "Core/Api.h"

#include "Input/InputState.h"

namespace Nightbird::Input
{
	class Provider;

	class NB_API System
	{
	public:
		System() = default;
		~System() = default;

		void Update(Provider& provider);

		bool IsDown(Digital input) const;
		bool WasPressed(Digital input) const;
		bool WasReleased(Digital input) const;
		
		float GetAxis1D(Analog1D axis) const;
		glm::vec2 GetAxis2D(Analog2D axis) const;

	private:
		State m_State;
	};
}
