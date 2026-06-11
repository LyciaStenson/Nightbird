#pragma once

#include "ImGuiWindow.h"

#include <string>
#include <vector>

//#include <rttr/type.h>

namespace Nightbird::Editor
{
	//struct SettingsCategory
	//{
		//std::string name;
		//std::vector<rttr::property> properties;
	//};

	class SettingsWindow : public ImGuiWindow
	{
	public:
		NB_TYPE()

		SettingsWindow(const std::string& title, bool open = false);

		//template<typename T>
		//void SetSettings(T& settings)
		//{
			//m_Settings = static_cast<void*>(&settings);
			//m_SettingsType = rttr::type::get<T>();
			//BuildCategories(rttr::instance(settings), m_Categories);
		//}

	protected:
		void OnRender() override;

	private:
		//void RenderCategory(const SettingsCategory& category);
		//void BuildCategories(rttr::instance settings, std::vector<SettingsCategory>& outCategories);
		
		//void* m_Settings = nullptr;
		//rttr::type m_SettingsType = rttr::type::get<void>();
		//std::vector<SettingsCategory> m_Categories;
		//int m_SelectedCategory = 0;
	};
}
