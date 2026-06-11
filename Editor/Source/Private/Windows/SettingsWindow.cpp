#include "Windows/SettingsWindow.h"

#include <unordered_map>

NB_REFLECT_NO_FIELDS(Nightbird::Editor::SettingsWindow, NB_PARENT(Nightbird::Editor::ImGuiWindow), NB_NO_FACTORY)

namespace Nightbird::Editor
{
	SettingsWindow::SettingsWindow(const std::string& title, bool open)
		: ImGuiWindow(title, open, { ImGuiWindowFlags_NoDocking })
	{

	}

	void SettingsWindow::OnRender()
	{
		//ImGui::BeginChild("LeftPanel", ImVec2(200, 0), true);
		//for (int i = 0; i < m_Categories.size(); ++i)
		//{
			//bool selected = (i == m_SelectedCategory);
			//if (ImGui::Selectable(m_Categories[i].name.c_str(), selected))
				//m_SelectedCategory = i;
		//}
		//ImGui::EndChild();

		//ImGui::SameLine();

		//ImGui::BeginChild("RightPanel", ImVec2(0, 0), false);
		//if (m_SelectedCategory >= 0 && m_SelectedCategory < m_Categories.size())
			//RenderCategory(m_Categories[m_SelectedCategory]);
		//ImGui::EndChild();
	}

	//void SettingsWindow::RenderCategory(const SettingsCategory& category)
	//{
	//	if (!m_Settings)
	//		return;

	//	rttr::instance settingInstance(m_Settings);

	//	for (auto& property : category.properties)
	//	{
	//		rttr::variant variant = property.get_value(settingInstance);
	//		std::string label = property.get_name().to_string();

	//		if (variant.is_type<bool>())
	//		{
	//			bool value = variant.get_value<bool>();
	//			if (ImGui::Checkbox(label.c_str(), &value))
	//				property.set_value(settingInstance, value);
	//		}
	//		else if (variant.is_type<int>())
	//		{
	//			int value = variant.get_value<int>();
	//			if (ImGui::DragInt(label.c_str(), &value))
	//				property.set_value(settingInstance, value);
	//		}
	//		else if (variant.is_type<float>())
	//		{
	//			float value = variant.get_value<float>();
	//			if (ImGui::DragFloat(label.c_str(), &value))
	//				property.set_value(settingInstance, value);
	//		}
	//		else if (variant.is_type<std::string>())
	//		{
	//			std::string value = variant.get_value<std::string>();
	//			char buffer[256];
	//			strncpy(buffer, value.c_str(), sizeof(buffer));
	//			buffer[255] = '\0';
	//			if (ImGui::InputText(label.c_str(), buffer, sizeof(buffer)))
	//				property.set_value(settingInstance, std::string(buffer));
	//		}
	//	}
	//}

	//void SettingsWindow::BuildCategories(rttr::instance settings, std::vector<SettingsCategory>& outCategories)
	//{
	//	outCategories.clear();
	//	rttr::type type = settings.get_type();

	//	std::unordered_map<std::string, SettingsCategory> categoryMap;

	//	for (auto& property : type.get_properties())
	//	{
	//		std::string categoryName = "General";
	//		auto metadata = property.get_metadata("Category");
	//		if (metadata.is_type<std::string>())
	//			categoryName = metadata.get_value<std::string>();

	//		categoryMap[categoryName].name = categoryName;
	//		categoryMap[categoryName].properties.push_back(property);
	//	}

	//	for (auto& [name, category] : categoryMap)
	//		outCategories.push_back(category);
	//}
}
