#include "Windows/AssetBrowser.h"

#include "EditorContext.h"

#include "Scene/TextSceneWriter.h"
#include "Import/TextCubemapWriter.h"

#include "Scene/TextSceneReader.h"
#include "Import/ImportManager.h"

#include "Core/Engine.h"
#include "Core/Platform.h"
#include "Core/Scene.h"
#include "Core/Cubemap.h"
#include "Core/Log.h"

NB_REFLECT_NO_FIELDS(Nightbird::Editor::AssetBrowser, NB_PARENT(Nightbird::Editor::ImGuiWindow), NB_NO_FACTORY)

namespace Nightbird::Editor
{
	static uuids::uuid GenerateUUID()
	{
		std::random_device randomDevice;

		auto seedData = std::array<int, std::mt19937::state_size>{};
		std::generate(std::begin(seedData), std::end(seedData), std::ref(randomDevice));
		std::seed_seq seq(std::begin(seedData), std::end(seedData));
		std::mt19937 generator(seq);
		uuids::uuid_random_generator gen{generator};

		return gen();
	}

	AssetBrowser::AssetBrowser(EditorContext& context, bool open)
		: ImGuiWindow("Asset Browser", open), m_Context(context)
	{
		m_Context.m_CurrentPath = context.GetImportManager().GetAssetsDir();
		std::filesystem::create_directories(m_Context.m_CurrentPath);
	}

	void AssetBrowser::OnRender()
	{
		if (m_Context.m_CurrentPath.has_parent_path())
		{
			if (ImGui::Button("Up"))
			{
				m_Context.m_CurrentPath = m_Context.m_CurrentPath.parent_path();
				m_Context.m_SelectedPath.clear();
			}
		}

		if (ImGui::IsKeyPressed(ImGuiKey_F2) && !m_Context.m_SelectedPath.empty() && m_Context.m_SelectedPath != m_RenamingPath)
		{
			m_RenamingPath = m_Context.m_SelectedPath;
			std::string filename = m_RenamingPath.filename().string();
			std::strncpy(m_RenameBuffer, filename.c_str(), sizeof(m_RenameBuffer) - 1);
			m_RenameBuffer[sizeof(m_RenameBuffer) - 1] = '\0';
			m_RenameFocusRequested = true;
		}

		for (const auto& entry : std::filesystem::directory_iterator(m_Context.m_CurrentPath))
		{
			const auto& path = entry.path();
			const std::string name = path.filename().string();
			bool selected = (path == m_Context.m_SelectedPath);
			bool isRenaming = (path == m_RenamingPath);

			ImGui::PushID(name.c_str());

			if (entry.is_directory())
			{
				if (isRenaming)
				{
					RenderRenameItem(path);
				}
				else
				{
					if (ImGui::Selectable(name.c_str(), selected))
					{
						m_Context.m_SelectedPath = path;
					}

					if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					{
						m_Context.m_CurrentPath = path;
						m_Context.m_SelectedPath.clear();
					}

					RenderItemContextMenu(path);
				}
			}
			else if (entry.is_regular_file())
			{
				if (isRenaming)
				{
					RenderRenameItem(path);
				}
				else
				{
					if (ImGui::Selectable(name.c_str(), selected))
					{
						m_Context.m_SelectedPath = path;
					}

					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoDisableHover | ImGuiDragDropFlags_SourceNoHoldToOpenOthers))
					{
						const AssetInfo* assetInfo = m_Context.GetImportManager().GetAssetInfo(path);
						if (assetInfo)
						{
							ImGui::SetDragDropPayload("ASSET_UUID", &assetInfo->uuid, sizeof(uuids::uuid));
							ImGui::Text(name.c_str());
						}
						ImGui::EndDragDropSource();
					}

					if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					{
						const AssetInfo* assetInfo = m_Context.GetImportManager().GetAssetInfo(m_Context.m_SelectedPath);
						if (assetInfo)
						{
							Core::SceneReadResult result = m_Context.GetImportManager().LoadScene(assetInfo->uuid);
							if (result.root)
							{
								auto scene = std::make_unique<Core::Scene>();
								scene->SetActiveCamera(result.activeCamera);

								for (auto& child : result.root->GetChildren())
									scene->GetRoot()->AddChild(std::move(child));

								m_Context.GetEngine().SetScene(std::move(scene));
							}
						}
					}

					RenderItemContextMenu(path);
				}
			}

			ImGui::PopID();
		}

		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered())
		{
			m_Context.m_SelectedPath.clear();
		}

		if (ImGui::BeginPopupContextWindow("NewContext", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::MenuItem("New Folder"))
			{
				std::filesystem::create_directory(m_Context.m_CurrentPath / "New Folder");
			}

			if (ImGui::MenuItem("New Scene"))
			{
				TextSceneWriter writer;
				Core::Scene scene;
				writer.Write(scene, GenerateUUID(), m_Context.m_CurrentPath / "NewScene.ntscene");
			}

			if (ImGui::MenuItem("New Cubemap"))
			{
				uuids::uuid uuid = GenerateUUID();
				std::filesystem::path outputPath = m_Context.m_CurrentPath / "NewCubemap.ntcubemap";

				AssetInfo assetInfo;
				assetInfo.uuid = uuid;
				assetInfo.importer = "text_cubemap";
				assetInfo.path = outputPath;

				static constexpr const char* s_FaceKeys[6] = {"pos_x", "neg_x", "pos_y", "neg_y", "pos_z", "neg_z"};
				for (const char* key : s_FaceKeys)
					assetInfo.tags[key] = {};

				TextCubemapWriter writer;
				writer.Write(assetInfo, outputPath);

				m_Context.GetImportManager().Register(std::move(assetInfo));
			}

			ImGui::EndPopup();
		}
	}

	void AssetBrowser::RenderItemContextMenu(const std::filesystem::path& path)
	{
		if (ImGui::BeginPopupContextItem("ItemContext"))
		{
			if (ImGui::MenuItem("Rename"))
			{
				m_RenamingPath = path;
				std::string filename = path.filename().string();
				std::strncpy(m_RenameBuffer, filename.c_str(), sizeof(m_RenameBuffer) - 1);
				m_RenameBuffer[sizeof(m_RenameBuffer) - 1] = '\0';
				m_RenameFocusRequested = true;
			}

			if (ImGui::MenuItem("Delete"))
			{
				std::filesystem::remove_all(path);
				if (m_Context.m_SelectedPath == path)
					m_Context.m_SelectedPath.clear();
			}

			ImGui::EndPopup();
		}
	}

	void AssetBrowser::RenderRenameItem(const std::filesystem::path& path)
	{
		if (m_RenameFocusRequested)
		{
			ImGui::SetKeyboardFocusHere();
			m_RenameFocusRequested = false;
		}

		ImGui::SetNextItemWidth(-FLT_MIN);
		bool confirmed = ImGui::InputText("##Rename", m_RenameBuffer, sizeof(m_RenameBuffer), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);

		bool cancelled = ImGui::IsItemFocused() && ImGui::IsKeyPressed(ImGuiKey_Escape);
		bool lostFocus = ImGui::IsItemDeactivated() && !confirmed && !cancelled;

		if (confirmed || lostFocus)
		{
			std::string newName = m_RenameBuffer;
			if (!newName.empty() && newName != path.filename().string())
			{
				std::filesystem::path newPath = path.parent_path() / newName;
				if (!std::filesystem::exists(newPath))
				{
					std::error_code errorCode;
					std::filesystem::rename(path, newPath, errorCode);
					if (!errorCode && m_Context.m_SelectedPath == path)
						m_Context.m_SelectedPath = newPath;
				}
			}

			m_RenamingPath.clear();
		}
		else if (cancelled)
		{
			m_RenamingPath.clear();
		}
	}
}
