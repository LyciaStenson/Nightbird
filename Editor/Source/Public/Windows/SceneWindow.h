#pragma once

#include "ViewWindow.h"

#include <memory>

namespace Nightbird::Core
{
	class RenderSurface;
	class Camera;
}

namespace Nightbird::Editor
{
	class EditorContext;
	
	class SceneWindow : public ViewWindow
	{
	public:
		NB_TYPE()
		SceneWindow(EditorContext& context, bool open = true);

	protected:
		void OnRender() override;

		Core::Camera* GetCamera() override;

	private:
		std::unique_ptr<Core::Camera> m_Camera;
		float m_MovementSpeed = 10.0f;
		float m_LookSensitivity = 0.005f;
		float m_LastX = 0.0f;
		float m_LastY = 0.0f;
	};
}