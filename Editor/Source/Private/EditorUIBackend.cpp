#include "EditorUIBackend.h"

#include "ImGuiPlatform.h"
#include "ImGuiRenderer.h"

#include <imgui.h>

namespace Nightbird::Editor
{
	EditorUIBackend::EditorUIBackend(std::unique_ptr<ImGuiPlatform> platform, std::unique_ptr<ImGuiRenderer> renderer)
		: m_Platform(std::move(platform)), m_Renderer(std::move(renderer))
	{

	}

	void EditorUIBackend::Initialize()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		m_Platform->Initialize();
		m_Renderer->Initialize();
	}

	void EditorUIBackend::Shutdown()
	{
		m_Renderer->Shutdown();
		m_Platform->Shutdown();

		ImGui::DestroyContext();
	}

	void EditorUIBackend::BeginFrame()
	{
		m_Platform->NewFrame();
		m_Renderer->NewFrame();
		ImGui::NewFrame();
	}

	void EditorUIBackend::EndFrame()
	{
		ImGui::Render();
		m_Renderer->RenderDrawData();
	}

	ImTextureID EditorUIBackend::RegisterSurface(Core::RenderSurface& surface)
	{
		return m_Renderer->RegisterSurface(surface);
	}
	
	void EditorUIBackend::UnregisterSurface(Core::RenderSurface& surface)
	{
		m_Renderer->UnregisterSurface(surface);
	}
}
