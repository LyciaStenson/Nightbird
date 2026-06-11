#pragma once

#include "ImGuiWindow.h"

#include <uuid.h>

namespace Nightbird::Editor
{
	class EditorContext;

	class BuildWindow : public ImGuiWindow
	{
	public:
		NB_TYPE()

		BuildWindow(EditorContext& context, bool open = false);

	protected:
		void OnRender() override;

	private:
		EditorContext& m_Context;

		int m_SelectedPlatform = 0;
		uuids::uuid m_SelectedSceneUUID;
		std::string m_SelectedSceneName;
	};
}
