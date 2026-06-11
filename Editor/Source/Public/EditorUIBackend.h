#pragma once

#include <imgui.h>

#include <memory>

namespace Nightbird::Core
{
	class RenderSurface;
}

namespace Nightbird::Editor
{
	class ImGuiPlatform;
	class ImGuiRenderer;

	class EditorUIBackend
	{
	public:
		EditorUIBackend(std::unique_ptr<ImGuiPlatform> platform, std::unique_ptr<ImGuiRenderer> renderer);
		~EditorUIBackend() = default;

		void Initialize();
		void Shutdown();
		void BeginFrame();
		void EndFrame();

		ImTextureID RegisterSurface(Core::RenderSurface& surface);
		void UnregisterSurface(Core::RenderSurface& surface);

	private:
		std::unique_ptr<ImGuiPlatform> m_Platform;
		std::unique_ptr<ImGuiRenderer> m_Renderer;
	};
}
