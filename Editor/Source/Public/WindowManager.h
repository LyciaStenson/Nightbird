#pragma once

#include "Core/TypeInfo.h"

#include "ImGuiWindow.h"

#include <memory>
#include <vector>

namespace Nightbird::Editor
{
	class WindowManager
	{
	public:
		template<typename T, typename... Args>
		T& AddWindow(Args&&... args)
		{
			auto window = std::make_unique<T>(std::forward<Args>(args)...);
			T& ref = *window;
			m_Windows.emplace_back(std::move(window));
			return ref;
		}

		template<typename T>
		T* GetWindow()
		{
			for (auto& window : m_Windows)
				if (auto casted = Cast<T>(window.get()))
					return casted;
			return nullptr;
		}

		void Render();

		const std::vector<std::unique_ptr<ImGuiWindow>>& GetWindows() const;

	private:
		std::vector<std::unique_ptr<ImGuiWindow>> m_Windows;
	};
}
