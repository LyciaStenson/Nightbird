#pragma once

#include "ImGuiWindow.h"

#include <filesystem>

namespace Nightbird::Editor
{
	class EditorContext;

	class Inspector : public ImGuiWindow
	{
	public:
		NB_TYPE()

		Inspector(EditorContext& context, bool open = true);

	protected:
		void OnRender() override;

	private:
		void DrawSceneObject(Core::SceneObject* object);
		void DrawFields(void* object, const TypeInfo* type);

		bool DrawAsset(const std::filesystem::path& path);

		EditorContext& m_Context;
	};
}
