#pragma once

#include "Core/RenderSurface.h"

namespace Nightbird::GX2
{
	class RenderSurface : public Core::RenderSurface
	{
	public:
		virtual void Begin() = 0;
		virtual void Finish() = 0;
	};
}
