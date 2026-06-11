#include "Scene/TextSceneReader.h"

#include "Import/AssetInfo.h"

#include "Core/SceneObject.h"
#include "Core/SpatialObject.h"
#include "Core/MeshInstance.h"
#include "Core/DirectionalLight.h"
#include "Core/PointLight.h"
#include "Core/Camera.h"
#include "Core/AudioSource.h"
#include "Core/Log.h"

namespace Nightbird::Editor
{
	Core::SceneReadResult TextSceneReader::Read(const std::filesystem::path& path)
	{
		Core::SceneReadResult result;

		if (!std::filesystem::exists(path))
		{
			Core::Log::Error("TextSceneReader: File not found: " + path.string());
			return result;
		}

		toml::parse_result tomlResult = toml::parse_file(path.string());
		if (!tomlResult)
		{
			Core::Log::Error("TextSceneReader: Failed to parse: " + path.string());
			return result;
		}

		toml::table& document = tomlResult.table();

		std::string sceneName = document["scene"]["name"].value_or(std::string{});
		std::string sceneUUIDString = document["scene"]["uuid"].value_or(std::string{});
		auto sceneUUID = uuids::uuid::from_string(sceneUUIDString);
		if (!sceneUUID)
		{
			Core::Log::Error("TextSceneReader: Invalid scene UUID in: " + path.string());
			return result;
		}

		result.root = std::make_unique<Core::SceneObject>();
		result.uuid = *sceneUUID;

		toml::array* nodesArray = document["nodes"].as_array();
		if (!nodesArray)
		{
			Core::Log::Warning("TextSceneReader: No nodes found in: " + path.string());
			return result;
		}

		std::unordered_map<uuids::uuid, Core::SceneObject*> nodeMap;
		std::unordered_map<uuids::uuid, std::unique_ptr<Core::SceneObject>> ownedNodesMap;

		for (auto& nodeValue : *nodesArray)
		{
			auto* nodeTable = nodeValue.as_table();
			if (!nodeTable)
				continue;

			std::string uuidString = (*nodeTable)["uuid"].value_or(std::string{});
			std::string name = (*nodeTable)["name"].value_or(std::string{});
			std::string typeName = (*nodeTable)["type"].value_or(std::string{});
			
			auto uuid = uuids::uuid::from_string(uuidString);
			if (!uuid)
			{
				Core::Log::Warning("TextSceneReader: Skipping node with invalid UUID");
				continue;
			}

			std::unique_ptr<Core::SceneObject> object;
			const TypeInfo* type = TypeRegistry::Find(typeName);
			if (type && type->HasFactory())
			{
				object.reset(type->CreateAs<Core::SceneObject>());
				object->SetName(type->name);
			}

			if (!object)
			{
				Core::Log::Warning("TextSceneReader: Unknown type: " + typeName + ", defaulting to SceneObject");
				object = std::make_unique<Core::SceneObject>();
				object->SetName(name);
			}

			if (auto* fieldsTable = (*nodeTable)["fields"].as_table())
				ReadFields(static_cast<void*>(object.get()), object->GetTypeInfo(), *fieldsTable);

			std::string sourceSceneUUIDString = (*nodeTable)["scene_uuid"].value_or(std::string{});
			if (!sourceSceneUUIDString.empty())
			{
				if (auto sourceSceneUUID = uuids::uuid::from_string(sourceSceneUUIDString))
					object->SetSourceSceneUUID(*sourceSceneUUID);
			}

			Core::SceneObject* objectPtr = object.get();
			nodeMap[*uuid] = objectPtr;
			ownedNodesMap[*uuid] = std::move(object);
		}

		for (auto& nodeValue : *nodesArray)
		{
			auto* nodeTable = nodeValue.as_table();
			if (!nodeTable)
				continue;

			std::string uuidString = (*nodeTable)["uuid"].value_or(std::string{});
			std::string parentString = (*nodeTable)["parent"].value_or(std::string{});

			auto uuid = uuids::uuid::from_string(uuidString);
			if (!uuid || !ownedNodesMap.count(*uuid))
				continue;

			if (!parentString.empty())
			{
				auto parentUUID = uuids::uuid::from_string(parentString);
				if (parentUUID && nodeMap.count(*parentUUID))
					nodeMap[*parentUUID]->AddChild(std::move(ownedNodesMap[*uuid]));
				else
					result.root->AddChild(std::move(ownedNodesMap[*uuid]));
			}
			else
			{
				result.root->AddChild(std::move(ownedNodesMap[*uuid]));
			}
		}

		std::string activeCameraUUIDString = document["scene"]["active_camera"].value_or(std::string{});
		auto activeCameraUUID = uuids::uuid::from_string(activeCameraUUIDString);
		if (activeCameraUUID && nodeMap.count(*activeCameraUUID))
			result.activeCamera = Cast<Core::Camera>(nodeMap[*activeCameraUUID]);

		Core::Log::Info("TextSceneReader: Loaded scene: " + sceneName);
		return result;
	}

	void TextSceneReader::ReadFields(void* object, const TypeInfo* type, const toml::table& table)
	{
		if (!object || !type)
			return;

		if (type->parent)
			ReadFields(object, type->parent, table);

		if (!type->HasFields())
			return;

		for (const FieldInfo* field = type->Begin(); field != type->End(); ++field)
		{
			switch (field->kind)
			{
			case FieldKind::Bool:
			{
				if (auto value = table[field->name].value<bool>())
					*field->GetPtrAs<bool>(object) = *value;
				break;
			}
			case FieldKind::Int32:
			{
				if (auto value = table[field->name].value<int32_t>())
					*field->GetPtrAs<int32_t>(object) = *value;
				break;
			}
			case FieldKind::UInt32:
			{
				if (auto value = table[field->name].value<uint32_t>())
					*field->GetPtrAs<uint32_t>(object) = *value;
				break;
			}
			case FieldKind::Float:
			{
				if (auto value = table[field->name].value<double>())
					*field->GetPtrAs<float>(object) = static_cast<float>(*value);
				break;
			}
			case FieldKind::String:
			{
				if (auto value = table[field->name].value<std::string>())
					*field->GetPtrAs<std::string>(object) = *value;
				break;
			}
			case FieldKind::Vector2:
			{
				if (auto* array = table[field->name].as_array(); array && array->size() >= 2)
				{
					auto* value = field->GetPtrAs<glm::vec2>(object);
					value->x = static_cast<float>(array->get(0)->value_or(0.0));
					value->y = static_cast<float>(array->get(1)->value_or(0.0));
				}
				break;
			}
			case FieldKind::Vector3:
			{
				if (auto* array = table[field->name].as_array(); array && array->size() >= 3)
				{
					auto* value = field->GetPtrAs<glm::vec3>(object);
					value->x = static_cast<float>(array->get(0)->value_or(0.0));
					value->y = static_cast<float>(array->get(1)->value_or(0.0));
					value->z = static_cast<float>(array->get(2)->value_or(0.0));
				}
				break;
			}
			case FieldKind::Vector4:
			{
				if (auto* array = table[field->name].as_array(); array && array->size() >= 4)
				{
					auto* value = field->GetPtrAs<glm::vec4>(object);
					value->x = static_cast<float>(array->get(0)->value_or(0.0));
					value->y = static_cast<float>(array->get(1)->value_or(0.0));
					value->z = static_cast<float>(array->get(2)->value_or(0.0));
					value->w = static_cast<float>(array->get(3)->value_or(0.0));
				}
				break;
			}
			case FieldKind::Quat:
			{
				if (auto* array = table[field->name].as_array(); array && array->size() >= 4)
				{
					auto* value = field->GetPtrAs<glm::vec4>(object);
					value->x = static_cast<float>(array->get(0)->value_or(0.0));
					value->y = static_cast<float>(array->get(1)->value_or(0.0));
					value->z = static_cast<float>(array->get(2)->value_or(0.0));
					value->w = static_cast<float>(array->get(3)->value_or(1.0));
				}
				break;
			}
			case FieldKind::UUID:
			{
				if (auto value = table[field->name].value<std::string>())
				{
					if (auto uuid = uuids::uuid::from_string(*value))
						*field->GetPtrAs<uuids::uuid>(object) = *uuid;
				}
				break;
			}
			case FieldKind::AssetRef:
			{
				if (auto value = table[field->name].value<std::string>())
				{
					if (auto uuid = uuids::uuid::from_string(*value))
						*field->GetPtrAs<uuids::uuid>(object) = *uuid;
				}
				break;
			}
			case FieldKind::Object:
			{
				if (field->type)
				{
					if (auto* nested = table[field->name].as_table())
						ReadFields(field->GetPtr(object), field->type, *nested);
				}
				break;
			}
			case FieldKind::Unknown:
				Core::Log::Info("TextSceneReader: Unknown FieldKind");
				break;
			default:
				Core::Log::Info("TextSceneReader: Unhandled FieldKind: " + std::to_string(static_cast<uint8_t>(field->kind)));
				break;
			}
		}
	}
}
