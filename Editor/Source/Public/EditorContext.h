#pragma once

#include <filesystem>

namespace Nightbird::Core
{
	class Engine;
	class SceneObject;
}

namespace Nightbird::Editor
{
	class EditorUIBackend;
	class ImportManager;
	class CookManager;

	class EditorContext
	{
	public:
		EditorContext(Core::Engine& engine, EditorUIBackend& editorUIBackend, ImportManager& importManager, CookManager& cookManager);

		void SelectObject(Core::SceneObject* object);
		void ClearSelection();

		Core::SceneObject* GetSelectedObject() const;

		Core::Engine& GetEngine();

		EditorUIBackend& GetEditorUIBackend() const;

		ImportManager& GetImportManager() const;
		CookManager& GetCookManager() const;

		std::filesystem::path m_SelectedPath;

	private:
		Core::Engine& m_Engine;
		EditorUIBackend& m_EditorUIBackend;
		ImportManager& m_ImportManager;
		CookManager& m_CookManager;
		
		Core::SceneObject* m_SelectedObject = nullptr;
	};
}
