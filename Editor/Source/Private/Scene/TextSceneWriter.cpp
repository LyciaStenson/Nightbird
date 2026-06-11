#include "Scene/TextSceneWriter.h"

#include "Core/Scene.h"
#include "Core/SceneObject.h"
#include "Core/Camera.h"
#include "Core/Log.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Nightbird::Editor
{
	void TextSceneWriter::Write(Core::Scene& scene, const std::string& sceneName,
		const uuids::uuid& sceneUUID, const std::filesystem::path& outputPath)
	{
		m_NodeUUIDs.clear();

		AssignNodeUUIDs(scene.GetRoot());

		toml::table document;

		toml::table sceneTable;
		sceneTable.insert("name", sceneName);
		sceneTable.insert("uuid", uuids::to_string(sceneUUID));
		const Core::Camera* activeCamera = scene.GetActiveCamera();
		sceneTable.insert("active_camera", activeCamera ? uuids::to_string(m_NodeUUIDs[activeCamera]) : std::string{});
		document.insert("scene", sceneTable);

		toml::array nodesArray;
		for (const auto& child : scene.GetRoot()->GetChildren())
			WriteNode(child.get(), nullptr, nodesArray);

		document.insert("nodes", nodesArray);

		std::filesystem::create_directories(outputPath.parent_path());
		std::ofstream file(outputPath);
		file << document;
		file.close();

		Core::Log::Info("Written .ntscene: " + outputPath.string());
	}

	void TextSceneWriter::AssignNodeUUIDs(Core::SceneObject* object)
	{
		if (!object)
			return;

		m_NodeUUIDs[object] = GenerateUUID();

		if (object->HasSourceScene())
			return;

		for (const auto& child : object->GetChildren())
			AssignNodeUUIDs(child.get());
	}

	void TextSceneWriter::WriteNode(Core::SceneObject* object, Core::SceneObject* parent, toml::array& nodesArray)
	{
		if (!object)
			return;

		toml::table node;
		
		node.insert("name", object->GetName());
		node.insert("uuid", uuids::to_string(m_NodeUUIDs[object]));
		node.insert("parent", parent ? uuids::to_string(m_NodeUUIDs[parent]) : std::string{});
		node.insert("type", object->GetTypeInfo()->name);

		toml::table fields;
		WriteFields(static_cast<void*>(object), object->GetTypeInfo(), fields);
		node.insert("fields", fields);

		if (object->HasSourceScene())
		{
			node.insert("scene_uuid", uuids::to_string(object->GetSourceSceneUUID().value()));
			nodesArray.push_back(node);
			return; // Do not serialize children of scene instance
		}

		nodesArray.push_back(node);

		for (const auto& child : object->GetChildren())
			WriteNode(child.get(), object, nodesArray);
	}

	void TextSceneWriter::WriteFields(void* object, const TypeInfo* type, toml::table& table)
	{
		if (!object || !type)
			return;

		if (type->parent)
			WriteFields(object, type->parent, table);

		if (!type->HasFields())
			return;

		for (const FieldInfo* field = type->Begin(); field != type->End(); ++field)
		{
			switch (field->kind)
			{
			case FieldKind::Bool:
				table.insert(field->name, *field->GetPtrAs<bool>(object));
				break;
			case FieldKind::Int32:
				table.insert(field->name, *field->GetPtrAs<int32_t>(object));
				break;
			case FieldKind::UInt32:
				table.insert(field->name, *field->GetPtrAs<uint32_t>(object));
				break;
			case FieldKind::Float:
				table.insert(field->name, *field->GetPtrAs<float>(object));
				break;
			case FieldKind::String:
				table.insert(field->name, *field->GetPtrAs<std::string>(object));
				break;
			case FieldKind::Vector2:
			{
				auto* vec = field->GetPtrAs<glm::vec2>(object);
				table.insert(field->name, toml::array{vec->x, vec->y});
				break;
			}
			case FieldKind::Vector3:
			{
				auto* vec = field->GetPtrAs<glm::vec3>(object);
				table.insert(field->name, toml::array{vec->x, vec->y, vec->z});
				break;
			}
			case FieldKind::Vector4:
			{
				auto* vec = field->GetPtrAs<glm::vec4>(object);
				table.insert(field->name, toml::array{vec->x, vec->y, vec->z, vec->w});
				break;
			}
			case FieldKind::Quat:
			{
				auto* quat = field->GetPtrAs<glm::quat>(object);
				table.insert(field->name, toml::array{ quat->x, quat->y, quat->z, quat->w });
				break;
			}
			case FieldKind::UUID:
			{
				auto* uuid = field->GetPtrAs<uuids::uuid>(object);
				table.insert(field->name, uuids::to_string(*uuid));
				break;
			}
			case FieldKind::AssetRef:
			{
				auto* uuid = field->GetPtrAs<uuids::uuid>(object);
				table.insert(field->name, uuids::to_string(*uuid));
				break;
			}
			case FieldKind::Object:
				if (field->name)
				{
					toml::table nested;
					WriteFields(field->GetPtr(object), field->type, nested);
					table.insert(field->name, nested);
				}
				break;
			case FieldKind::Unknown:
				Core::Log::Info("TextSceneWriter: Unknown FieldKind");
				break;
			default:
				Core::Log::Info("TextSceneWriter: Unhandled FieldKind: " + std::to_string(static_cast<uint8_t>(field->kind)));
				break;
			}
		}
	}
	
	uuids::uuid TextSceneWriter::GenerateUUID() const
	{
		std::random_device randomDevice;

		auto seedData = std::array<int, std::mt19937::state_size>{};
		std::generate(std::begin(seedData), std::end(seedData), std::ref(randomDevice));
		std::seed_seq seq(std::begin(seedData), std::end(seedData));
		std::mt19937 generator(seq);
		uuids::uuid_random_generator gen{generator};

		return gen();
	}
}
