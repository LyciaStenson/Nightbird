#pragma once

namespace Nightbird::Editor
{
	class ImGuiPlatform
	{
	public:
		virtual ~ImGuiPlatform() = default;

		virtual void Initialize() = 0;
		virtual void Shutdown() = 0;
		virtual void NewFrame() = 0;
	};
}
