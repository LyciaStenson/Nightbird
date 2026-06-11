#include "Windows/SceneOutliner.h"

#include "EditorContext.h"
#include "Import/ImportManager.h"

#include "Core/Engine.h"
#include "Core/Scene.h"
#include "Core/SceneObject.h"
#include "Core/Log.h"

NB_REFLECT_NO_FIELDS(Nightbird::Editor::SceneOutliner, NB_PARENT(Nightbird::Editor::ImGuiWindow), NB_NO_FACTORY)

namespace Nightbird::Editor
{
	SceneOutliner::SceneOutliner(EditorContext& context, bool open)
		: ImGuiWindow("Scene Outliner", open), m_Context(context)
	{

	}

	void SceneOutliner::OnRender()
	{
		DrawAddObjectPopup();

		ImGui::Dummy(ImVec2(0.0f, 1.0f));
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0.0f, 1.0f));

		bool dropHandled = false;

		for (const auto& child : m_Context.GetEngine().GetScene().GetRoot()->GetChildren())
			DrawSceneNode(child.get(), dropHandled);

		ImVec2 space = ImGui::GetContentRegionAvail();
		ImGui::Dummy(ImVec2(space.x, std::max(24.0f, space.y)));

		if (!dropHandled && ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_OBJECT"))
			{
				Core::SceneObject* received = *static_cast<Core::SceneObject**>(payload->Data);
				if (received)
					received->SetParent(m_Context.GetEngine().GetScene().GetRoot());
			}
			else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_UUID"))
			{
				const uuids::uuid* droppedUUID = static_cast<const uuids::uuid*>(payload->Data);
				if (droppedUUID && !droppedUUID->is_nil())
				{
					Core::SceneReadResult result = m_Context.GetImportManager().LoadScene(*droppedUUID);
					result.root->SetSourceSceneUUID(*droppedUUID);
					m_Context.GetEngine().GetScene().GetRoot()->AddChild(std::move(result.root));
				}
			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered())
		{
			m_Context.ClearSelection();
		}
	}

	void SceneOutliner::DrawAddObjectPopup()
	{
		if (ImGui::Button("Add"))
			ImGui::OpenPopup("AddObject");
		
		if (ImGui::BeginPopup("AddObject"))
		{
			for (const TypeInfo* type : TypeRegistry::GetAll())
			{
				if (type->IsA(&Core::SceneObject::s_TypeInfo) && type->HasFactory())
				{
					if (ImGui::MenuItem(type->name))
					{
						auto* rawObject = type->CreateAs<Core::SceneObject>();
						if (rawObject)
						{
							rawObject->SetName(type->name);
							std::unique_ptr<Core::SceneObject> object(rawObject);

							if (auto* selectObject = m_Context.GetSelectedObject())
								selectObject->AddChild(std::move(object));
							else
								m_Context.GetEngine().GetScene().GetRoot()->AddChild(std::move(object));
						}
					}
				}
			}
			ImGui::EndPopup();
		}
	}

	void SceneOutliner::DrawSceneNode(Core::SceneObject* object, bool& dropHandled)
	{
		if (!object)
			return;

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
		if (object == m_Context.GetSelectedObject())
			flags |= ImGuiTreeNodeFlags_Selected;

		if (object->GetChildren().empty() || object->HasSourceScene())
			flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

		bool opened = ImGui::TreeNodeEx(object, flags, "%s", object->GetName().c_str());

		if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			m_Context.SelectObject(object);

		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("SCENE_OBJECT", &object, sizeof(Core::SceneObject*));
			ImGui::Text(object->GetName().c_str());
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_OBJECT"))
			{
				Core::SceneObject* received = *static_cast<Core::SceneObject**>(payload->Data);
				if (received && received != object)
					received->SetParent(object);
			}
			else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_UUID"))
			{
				const uuids::uuid* droppedUUID = static_cast<const uuids::uuid*>(payload->Data);
				if (droppedUUID && !droppedUUID->is_nil())
				{
					Core::SceneReadResult result = m_Context.GetImportManager().LoadScene(*droppedUUID);
					result.root->SetSourceSceneUUID(*droppedUUID);
					object->AddChild(std::move(result.root));
				}
			}
			ImGui::EndDragDropTarget();
		}

		if (opened && !(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
		{
			for (auto& child : object->GetChildren())
			{
				DrawSceneNode(child.get(), dropHandled);
			}
			ImGui::TreePop();
		}
	}
}
