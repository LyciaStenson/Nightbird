#pragma once

#include <glm/glm.hpp>

#include <array>

namespace Nightbird::Input
{
	enum class Digital : uint16_t
	{
		Key_Space, Key_Enter, Key_Escape,
		Key_A, Key_B, Key_C, Key_D, Key_E, Key_F, Key_G, Key_H, Key_I, Key_J, Key_K, Key_L, Key_M, Key_N, Key_O, Key_P, Key_Q, Key_R, Key_S, Key_T, Key_U, Key_V, Key_W, Key_X, Key_Y, Key_Z,
		Mouse_Left, Mouse_Right, Mouse_Middle,
		Pad_A, Pad_B, Pad_X, Pad_Y, Pad_L1, Pad_R1, Pad_L2, Pad_R2,
		Pad_L = Pad_L1, Pad_R = Pad_R1,
		Pad_ZL = Pad_L2, Pad_ZR = Pad_R2,
		Pad_Start,
		Pad_Plus = Pad_Start,
		Pad_Back,
		Pad_Minus = Pad_Back,
		Pad_Select = Pad_Back,
		Pad_Home,
		Pad_Up, Pad_Down, Pad_Left, Pad_Right,
		COUNT
	};
	
	enum class Analog1D : uint16_t
	{
		Mouse_Wheel,
		COUNT
	};

	enum class Analog2D : uint16_t
	{
		Mouse_Move,
		Pad_LeftStick, Pad_RightStick,
		COUNT
	};

	struct State
	{
		std::array<bool, static_cast<size_t>(Digital::COUNT)> down{};
		std::array<bool, static_cast<size_t>(Digital::COUNT)> pressed{};
		std::array<bool, static_cast<size_t>(Digital::COUNT)> released{};
		
		std::array<float, static_cast<size_t>(Analog1D::COUNT)> axes1D{};
		std::array<glm::vec2, static_cast<size_t>(Analog2D::COUNT)> axes2D{};
	};
}
