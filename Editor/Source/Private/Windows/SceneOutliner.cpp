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
		ImVec2 itemMin = ImGui::GetItemRectMin();
		ImVec2 itemMax = ImGui::GetItemRectMax();
		float itemHeight = itemMax.y - itemMin.y;
		
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
			float mouseY = ImGui::GetMousePos().y;
			float relativeY = (mouseY - itemMin.y) / itemHeight;

			enum class DropZone { Before, Into, After };
			DropZone zone = DropZone::Into;
			if (relativeY < 0.25f)
				zone = DropZone::Before;
			else if (relativeY > 0.75f)
				zone = DropZone::After;

			if (zone != DropZone::Into)
			{
				float lineY = (zone == DropZone::Before) ? itemMin.y : itemMax.y;
				float fullRight = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

				ImU32 dragDropTargetColor = ImGui::GetColorU32(ImGuiCol_DragDropTarget);
				ImGui::PushStyleColor(ImGuiCol_DragDropTarget, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

				ImGui::GetWindowDrawList()->AddLine(ImVec2(itemMin.x, lineY), ImVec2(fullRight, lineY), dragDropTargetColor, 2.0f);
			}

			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_OBJECT"))
			{
				Core::SceneObject* received = *static_cast<Core::SceneObject**>(payload->Data);
				if (received && received != object)
				{
					if (zone == DropZone::Into)
					{
						if (!object->HasSourceScene())
							received->SetParent(object);
					}
					else
					{
						Core::SceneObject* targetParent = object->GetParent();
						if (targetParent && received != targetParent)
						{
							int index = targetParent->GetChildIndex(object);
							if (zone == DropZone::After)
								index += 1;
							received->SetParent(targetParent, index);
						}
					}
				}
				dropHandled = true;
			}
			else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_UUID"))
			{
				const uuids::uuid* droppedUUID = static_cast<const uuids::uuid*>(payload->Data);
				if (droppedUUID && !droppedUUID->is_nil())
				{
					Core::SceneReadResult result = m_Context.GetImportManager().LoadScene(*droppedUUID);
					result.root->SetSourceSceneUUID(*droppedUUID);

					if (zone == DropZone::Into)
					{
						object->AddChild(std::move(result.root));
					}
					else
					{
						Core::SceneObject* targetParent = object->GetParent();
						int index = targetParent ? targetParent->GetChildIndex(object) : -1;
						if (zone == DropZone::After && index >= 0)
							index += 1;
						if (targetParent)
							targetParent->AddChild(std::move(result.root), index);
						else
							object->AddChild(std::move(result.root));
					}
				}
				dropHandled = true;
			}

			if (zone != DropZone::Into)
				ImGui::PopStyleColor();
			
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
