#include "WindowManager.h"

namespace Nightbird::Editor
{
	void WindowManager::Render()
	{
		for (auto& window : m_Windows)
			window->Render();
	}

	const std::vector<std::unique_ptr<ImGuiWindow>>& WindowManager::GetWindows() const
	{
		return m_Windows;
	}
}