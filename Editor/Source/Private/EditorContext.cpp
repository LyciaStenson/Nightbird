#include "EditorContext.h"

#include "EditorUIBackend.h"

#include "Import/ImportManager.h"
#include "Cook/CookManager.h"

#include "Core/SceneObject.h"

namespace Nightbird::Editor
{
	EditorContext::EditorContext(Core::Engine& engine, EditorUIBackend& editorUIBackend, ImportManager& importManager, CookManager& cookManager)
		: m_Engine(engine), m_EditorUIBackend(editorUIBackend), m_ImportManager(importManager), m_CookManager(cookManager)
	{

	}

	void EditorContext::SelectObject(Core::SceneObject* object)
	{
		m_SelectedObject = object;
	}

	void EditorContext::ClearSelection()
	{
		m_SelectedObject = nullptr;
	}

	Core::SceneObject* EditorContext::GetSelectedObject() const
	{
		return m_SelectedObject;
	}

	Core::Engine& EditorContext::GetEngine()
	{
		return m_Engine;
	}

	EditorUIBackend& EditorContext::GetEditorUIBackend() const
	{
		return m_EditorUIBackend;
	}

	ImportManager& EditorContext::GetImportManager() const
	{
		return m_ImportManager;
	}

	CookManager& EditorContext::GetCookManager() const
	{
		return m_CookManager;
	}
}
