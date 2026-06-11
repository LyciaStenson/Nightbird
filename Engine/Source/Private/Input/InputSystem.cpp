#include "Input/InputSystem.h"

#include "Input/InputProvider.h"

#include <glm/glm.hpp>

namespace Nightbird::Input
{
	void System::Update(Provider& provider)
	{
		provider.Poll(m_State);
	}

	bool System::IsDown(Digital input) const
	{
		return m_State.down[static_cast<size_t>(input)];
	}

	bool System::WasPressed(Digital input) const
	{
		return m_State.pressed[static_cast<size_t>(input)];
	}
	
	bool System::WasReleased(Digital input) const
	{
		return m_State.released[static_cast<size_t>(input)];
	}

	float System::GetAxis1D(Analog1D axis) const
	{
		return m_State.axes1D[static_cast<size_t>(axis)];
	}
	
	glm::vec2 System::GetAxis2D(Analog2D axis) const
	{
		return m_State.axes2D[static_cast<size_t>(axis)];
	}
}
