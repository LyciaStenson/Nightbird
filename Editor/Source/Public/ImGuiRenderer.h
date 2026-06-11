#pragma once

#include <imgui.h>

namespace Nightbird::Core
{
	class RenderSurface;
}

namespace Nightbird::Editor
{
	class ImGuiRenderer
	{
	public:
		virtual ~ImGuiRenderer() = default;

		virtual void Initialize() = 0;
		virtual void Shutdown() = 0;
		virtual void NewFrame() = 0;
		virtual void RenderDrawData() = 0;
		virtual ImTextureID RegisterSurface(Core::RenderSurface& surface) = 0;
		virtual void UnregisterSurface(Core::RenderSurface& surface) = 0;
	};
}
