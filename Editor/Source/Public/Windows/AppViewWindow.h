#pragma once

#include "ImGuiWindow.h"

#include <memory>

namespace Nightbird::Core
{
	class RenderSurface;
}

namespace Nightbird::Editor
{
	class EditorContext;

	class AppViewWindow : public ImGuiWindow
	{
	public:
		NB_TYPE()
		AppViewWindow(EditorContext& context, bool open = true);
		~AppViewWindow();

		Core::RenderSurface& GetSurface();

		uint32_t GetPendingWidth() const;
		uint32_t GetPendingHeight() const;
		bool NeedsResize() const;
		void Resize(uint32_t width, uint32_t height);

	protected:
		void OnRender() override;

	private:
		EditorContext& m_Context;

		std::unique_ptr<Core::RenderSurface> m_Surface;

		ImTextureID m_TextureId = 0;
		
		uint32_t m_CurrentWidth = 800;
		uint32_t m_CurrentHeight = 600;
		uint32_t m_PendingWidth = 800;
		uint32_t m_PendingHeight = 600;
		bool m_NeedsResize = false;
	};
}
