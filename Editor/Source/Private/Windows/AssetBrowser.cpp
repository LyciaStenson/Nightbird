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
		m_CurrentPath = context.GetImportManager().GetAssetsDir();
	}

	void AssetBrowser::OnRender()
	{
		if (m_CurrentPath.has_parent_path())
		{
			if (ImGui::Button("Up"))
			{
				m_CurrentPath = m_CurrentPath.parent_path();
				m_Context.m_SelectedPath.clear();
			}
		}

		for (const auto& entry : std::filesystem::directory_iterator(m_CurrentPath))
		{
			const auto& path = entry.path();
			const std::string name = path.filename().string();

			bool selected = path == m_Context.m_SelectedPath;

			if (entry.is_directory())
			{
				if (ImGui::Selectable(name.c_str(), selected))
				{
					m_Context.m_SelectedPath = path;
				}
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					m_CurrentPath = path;
					m_Context.m_SelectedPath.clear();
				}
			}
			else if (entry.is_regular_file())
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
			}
		}

		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered())
		{
			m_Context.m_SelectedPath.clear();
		}

		if (ImGui::BeginPopupContextWindow("TestPopup", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::MenuItem("New Folder"))
			{
				std::filesystem::create_directory(m_CurrentPath / "New Folder");
			}

			if (ImGui::MenuItem("New Scene"))
			{
				TextSceneWriter writer;
				Core::Scene scene;
				writer.Write(scene, "NewScene", GenerateUUID(), m_CurrentPath / "NewScene.ntscene");
			}

			if (ImGui::MenuItem("New Cubemap"))
			{
				uuids::uuid uuid = GenerateUUID();
				std::filesystem::path outputPath = m_CurrentPath / "NewCubemap.ntcubemap";

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
}
