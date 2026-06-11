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
	
	class SceneWindow : public ImGuiWindow
	{
	public:
		NB_TYPE()
		SceneWindow(EditorContext& context, bool open = true);
		~SceneWindow();

		Core::RenderSurface& GetSurface();
		Core::Camera& GetCamera();
		
		uint32_t GetPendingWidth() const;
		uint32_t GetPendingHeight() const;
		bool NeedsResize() const;
		void Resize(uint32_t width, uint32_t height);

	protected:
		void OnRender() override;

	private:
		EditorContext& m_Context;

		std::unique_ptr<Core::RenderSurface> m_Surface;

		std::unique_ptr<Core::Camera> m_Camera;
		float m_MovementSpeed = 10.0f;
		float m_LookSensitivity = 0.005f;
		float m_LastX = 0.0f;
		float m_LastY = 0.0f;

		ImTextureID m_TextureId = 0;

		uint32_t m_CurrentWidth = 800;
		uint32_t m_CurrentHeight = 600;
		uint32_t m_PendingWidth = 800;
		uint32_t m_PendingHeight = 600;
		bool m_NeedsResize = false;
	};
}