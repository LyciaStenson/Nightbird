#pragma once

#include "ImGuiWindow.h"

#include <memory>

namespace Nightbird::Core
{
	class RenderSurface;
	class Camera;
}

namespace Nightbird::Editor
{
	class EditorContext;

	class ViewWindow : public ImGuiWindow
	{
	public:
		NB_TYPE()
		ViewWindow(EditorContext& context, const std::string& title, bool open = true);
		~ViewWindow();

		Core::RenderSurface& GetSurface();

	protected:
		virtual void OnRender() override;

		virtual Core::Camera* GetCamera() = 0;

		void Resize(uint32_t width, uint32_t height);

		EditorContext& m_Context;

	private:
		std::unique_ptr<Core::RenderSurface> m_Surface;

		ImTextureID m_TextureId = 0;

		uint32_t m_CurrentWidth = 800;
		uint32_t m_CurrentHeight = 600;
		uint32_t m_PendingWidth = 800;
		uint32_t m_PendingHeight = 600;
		bool m_NeedsResize = false;
	};
}
