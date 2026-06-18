#include "Windows/AppViewWindow.h"

#include "EditorContext.h"
#include "EditorUIBackend.h"

#include "Core/Engine.h"
#include "Core/Renderer.h"
#include "Core/RenderSurface.h"

NB_REFLECT_NO_FIELDS(Nightbird::Editor::AppViewWindow, NB_PARENT(Nightbird::Editor::ImGuiWindow), NB_NO_FACTORY)

namespace Nightbird::Editor
{
	AppViewWindow::AppViewWindow(EditorContext& context, bool open)
		: ImGuiWindow("App View", open, { .flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse, .padding = ImVec2(4.0f, 4.0f) }), m_Context(context)
	{
		m_Surface = context.GetEngine().GetRenderer().CreateOffscreenSurface(m_CurrentWidth, m_CurrentHeight, Core::RenderSurfaceFormat::Default);
		m_TextureId = m_Context.GetEditorUIBackend().RegisterSurface(*m_Surface);
	}

	AppViewWindow::~AppViewWindow()
	{
		if (m_TextureId)
			m_Context.GetEditorUIBackend().UnregisterSurface(*m_Surface);
	}
	
	Core::RenderSurface& AppViewWindow::GetSurface()
	{
		return *m_Surface;
	}

	uint32_t AppViewWindow::GetPendingWidth() const
	{
		return m_PendingWidth;
	}

	uint32_t AppViewWindow::GetPendingHeight() const
	{
		return m_PendingHeight;
	}

	bool AppViewWindow::NeedsResize() const
	{
		return m_NeedsResize;
	}

	void AppViewWindow::Resize(uint32_t width, uint32_t height)
	{
		m_Context.GetEditorUIBackend().UnregisterSurface(*m_Surface);

		m_Surface->Resize(width, height);

		m_TextureId = m_Context.GetEditorUIBackend().RegisterSurface(*m_Surface);

		m_CurrentWidth = width;
		m_CurrentHeight = height;
		m_NeedsResize = false;
	}

	void AppViewWindow::OnRender()
	{
		ImVec2 size = ImGui::GetContentRegionAvail();
		uint32_t newWidth = static_cast<uint32_t>(std::max(1.0f, size.x));
		uint32_t newHeight = static_cast<uint32_t>(std::max(1.0f, size.y));

		if (newWidth != m_CurrentWidth || newHeight != m_CurrentHeight)
		{
			m_PendingWidth = newWidth;
			m_PendingHeight = newHeight;
			m_NeedsResize = true;
		}

		ImGui::Image(m_TextureId, ImVec2(static_cast<float>(m_CurrentWidth), static_cast<float>(m_CurrentHeight)));
	}
}
