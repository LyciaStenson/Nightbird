#include "Windows/SceneWindow.h"

#include "EditorContext.h"
#include "EditorUIBackend.h"

#include "Core/Engine.h"
#include "Core/Renderer.h"
#include "Core/RenderSurface.h"
#include "Core/Scene.h"
#include "Core/SceneObject.h"
#include "Core/Camera.h"
#include "Core/Log.h"

NB_REFLECT_NO_FIELDS(Nightbird::Editor::SceneWindow, NB_PARENT(Nightbird::Editor::ImGuiWindow), NB_NO_FACTORY)

namespace Nightbird::Editor
{
	SceneWindow::SceneWindow(EditorContext& context, bool open)
		: ImGuiWindow("Scene", open, { .flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse, .padding = ImVec2(4.0f, 4.0f) }), m_Context(context)
	{
		m_Surface = context.GetEngine().GetRenderer().CreateOffscreenSurface(m_CurrentWidth, m_CurrentHeight, Core::RenderSurfaceFormat::Default);

		m_TextureId = m_Context.GetEditorUIBackend().RegisterSurface(*m_Surface);

		m_Camera = std::make_unique<Core::Camera>();
	}

	SceneWindow::~SceneWindow()
	{
		if (m_TextureId)
			m_Context.GetEditorUIBackend().UnregisterSurface(*m_Surface);
	}

	Core::RenderSurface& SceneWindow::GetSurface()
	{
		return *m_Surface;
	}

	Core::Camera& SceneWindow::GetCamera()
	{
		return *m_Camera;
	}

	uint32_t SceneWindow::GetPendingWidth() const
	{
		return m_PendingWidth;
	}

	uint32_t SceneWindow::GetPendingHeight() const
	{
		return m_PendingHeight;
	}

	bool SceneWindow::NeedsResize() const
	{
		return m_NeedsResize;
	}

	void SceneWindow::Resize(uint32_t width, uint32_t height)
	{
		m_Context.GetEditorUIBackend().UnregisterSurface(*m_Surface);

		m_Surface->Resize(width, height);

		m_TextureId = m_Context.GetEditorUIBackend().RegisterSurface(*m_Surface);

		m_CurrentWidth = width;
		m_CurrentHeight = height;
		m_NeedsResize = false;
	}

	void SceneWindow::OnRender()
	{
		static bool rightMouseHeld = false;
		static bool firstFrame = true;

		auto& inputSystem = m_Context.GetEngine().GetInputSystem();

		if (ImGui::IsWindowHovered() && inputSystem.WasPressed(Input::Digital::Mouse_Right))
		{
			rightMouseHeld = true;
			firstFrame = true;
		}

		if (inputSystem.WasReleased(Input::Digital::Mouse_Right))
			rightMouseHeld = false;

		if (rightMouseHeld && inputSystem.IsDown(Input::Digital::Mouse_Right))
		{
			if (firstFrame)
			{
				ImVec2 mousePos = ImGui::GetMousePos();
				m_LastX = mousePos.x;
				m_LastY = mousePos.y;
				firstFrame = false;
			}

			float mouseWheel = inputSystem.GetAxis1D(Input::Analog1D::Mouse_Wheel);
			m_MovementSpeed += mouseWheel;
			m_MovementSpeed = glm::clamp(m_MovementSpeed, 1.0f, 30.0f);
			
			glm::vec3 forward = m_Camera->m_Transform.rotation * glm::vec3(0.0f, 0.0f, -1.0f);
			glm::vec3 right = m_Camera->m_Transform.rotation * glm::vec3(1.0f, 0.0f, 0.0f);
			glm::vec3 up = m_Camera->m_Transform.rotation * glm::vec3(0.0f, 1.0f, 0.0f);

			glm::vec3 movementDir(0.0f);

			if (inputSystem.IsDown(Input::Digital::Key_W) || inputSystem.IsDown(Input::Digital::Pad_Up))
				movementDir += forward;
			if (inputSystem.IsDown(Input::Digital::Key_S) || inputSystem.IsDown(Input::Digital::Pad_Down))
				movementDir -= forward;
			if (inputSystem.IsDown(Input::Digital::Key_D) || inputSystem.IsDown(Input::Digital::Pad_Right))
				movementDir += right;
			if (inputSystem.IsDown(Input::Digital::Key_A) || inputSystem.IsDown(Input::Digital::Pad_Left))
				movementDir -= right;
			if (inputSystem.IsDown(Input::Digital::Key_E) || inputSystem.IsDown(Input::Digital::Pad_A))
				movementDir += up;
			if (inputSystem.IsDown(Input::Digital::Key_Q) || inputSystem.IsDown(Input::Digital::Pad_B))
				movementDir -= up;

			m_Camera->m_Transform.position += movementDir * m_MovementSpeed * m_Context.GetEngine().GetDeltaTime();

			ImVec2 mousePos = ImGui::GetMousePos();
			float deltaX = m_LastX - mousePos.x;
			float deltaY = m_LastY - mousePos.y;

			m_LastX = mousePos.x;
			m_LastY = mousePos.y;

			float yawDelta = deltaX * m_LookSensitivity;
			float pitchDelta = deltaY * m_LookSensitivity;

			glm::quat pitchQuat = glm::angleAxis(pitchDelta, right);
			glm::quat yawQuat = glm::angleAxis(yawDelta, glm::vec3(0.0f, 1.0f, 0.0f));

			m_Camera->m_Transform.rotation = glm::normalize(yawQuat * pitchQuat * m_Camera->m_Transform.rotation);
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
