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

NB_REFLECT_NO_FIELDS(Nightbird::Editor::SceneWindow, NB_PARENT(Nightbird::Editor::ViewWindow), NB_NO_FACTORY)

namespace Nightbird::Editor
{
	SceneWindow::SceneWindow(EditorContext& context, bool open)
		: ViewWindow(context, "Scene", open)
	{
		m_Camera = std::make_unique<Core::Camera>();
	}

	Core::Camera* SceneWindow::GetCamera()
	{
		return m_Camera.get();
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

		ViewWindow::OnRender();
	}
}
