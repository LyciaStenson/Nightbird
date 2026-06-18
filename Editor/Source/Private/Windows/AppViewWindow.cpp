#include "Windows/AppViewWindow.h"

#include "EditorContext.h"
#include "EditorUIBackend.h"

#include "Core/Engine.h"
#include "Core/Renderer.h"
#include "Core/RenderSurface.h"
#include "Core/Scene.h"

NB_REFLECT_NO_FIELDS(Nightbird::Editor::AppViewWindow, NB_PARENT(Nightbird::Editor::ViewWindow), NB_NO_FACTORY)

namespace Nightbird::Editor
{
	AppViewWindow::AppViewWindow(EditorContext& context, bool open)
		: ViewWindow(context, "App View", open)
	{

	}

	Core::Camera* AppViewWindow::GetCamera()
	{
		return m_Context.GetEngine().GetScene().GetActiveCamera();
	}
}
