#pragma once

#include "ImGuiWindow.h"

namespace Nightbird::Core
{
	class SceneObject;
}

namespace Nightbird::Editor
{
	class EditorContext;

	class SceneOutliner : public ImGuiWindow
	{
	public:
		NB_TYPE()
		SceneOutliner(EditorContext& context, bool open = true);
	
	protected:
		void OnRender() override;

	private:
		void DrawAddObjectPopup();
		void DrawSceneNode(Core::SceneObject* object, bool& dropHandled);

		EditorContext& m_Context;
	};
}
