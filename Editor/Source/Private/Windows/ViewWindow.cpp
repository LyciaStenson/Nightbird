#include "Windows/ViewWindow.h"

#include "EditorContext.h"
#include "EditorUIBackend.h"

#include "Core/Engine.h"
#include "Core/Renderer.h"
#include "Core/RenderSurface.h"
#include "Core/Camera.h"

NB_REFLECT_NO_FIELDS(Nightbird::Editor::ViewWindow, NB_PARENT(Nightbird::Editor::ImGuiWindow), NB_NO_FACTORY)

namespace Nightbird::Editor
{
	ViewWindow::ViewWindow(EditorContext& context, const std::string& name, bool open)
		: ImGuiWindow(name, open, {.flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse, .padding = ImVec2(4.0f, 4.0f)}), m_Context(context)
	{
		m_Surface = context.GetEngine().GetRenderer().CreateOffscreenSurface(m_CurrentWidth, m_CurrentHeight, Core::RenderSurfaceFormat::Default);
		m_TextureId = m_Context.GetEditorUIBackend().RegisterSurface(*m_Surface);
	}

	ViewWindow::~ViewWindow()
	{
		if (m_TextureId)
			m_Context.GetEditorUIBackend().UnregisterSurface(*m_Surface);
	}

	Core::RenderSurface& ViewWindow::GetSurface()
	{
		return *m_Surface;
	}
	
	void ViewWindow::Resize(uint32_t width, uint32_t height)
	{
		m_Context.GetEditorUIBackend().UnregisterSurface(*m_Surface);

		m_Surface->Resize(width, height);

		m_TextureId = m_Context.GetEditorUIBackend().RegisterSurface(*m_Surface);

		m_CurrentWidth = width;
		m_CurrentHeight = height;
		m_NeedsResize = false;
	}

	void ViewWindow::OnRender()
	{
		if (m_NeedsResize)
			Resize(m_PendingWidth, m_PendingHeight);
		
		Core::Engine& engine = m_Context.GetEngine();
		Core::Renderer& renderer = engine.GetRenderer();

		Core::Camera* camera = GetCamera();
		if (camera)
			engine.GetRenderer().SubmitScene(engine.GetScene(), *camera);

		if (m_Surface)
		{
			renderer.BeginFrame(*m_Surface);
			renderer.DrawScene(*m_Surface);
			renderer.EndFrame(*m_Surface);
		}

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
