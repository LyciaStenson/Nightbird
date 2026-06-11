#include "Scene/BinarySceneWriter.h"

#include "Core/Transform.h"
#include "Core/Scene.h"
#include "Core/SceneObject.h"
#include "Core/SpatialObject.h"
#include "Core/Camera.h"
#include "Core/MeshInstance.h"
#include "Core/DirectionalLight.h"
#include "Core/PointLight.h"
#include "Core/AudioSource.h"
#include "Core/Log.h"

#include "Cook/BinaryWriter.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Nightbird::Editor
{
	void BinarySceneWriter::Write(Core::SceneObject* root, const uuids::uuid& sceneUUID,
		const std::filesystem::path& outputPath, Endianness endianness,
		Core::Camera* activeCamera)
	{
		m_NodeUUIDs.clear();

		AssignNodeUUIDs(root, true);

		std::filesystem::create_directories(outputPath.parent_path());
		BinaryWriter writer(outputPath, endianness);

		// Type
		writer.WriteUInt8('S');
		writer.WriteUInt8('C');
		writer.WriteUInt8('N');
		writer.WriteUInt8('E');

		// Version
		writer.WriteUInt32(1);

		// Scene name
		std::string name = root->GetName();
		writer.WriteUInt32(static_cast<uint32_t>(name.size()));
		writer.WriteRawBytes(reinterpret_cast<const uint8_t*>(name.data()), name.size());

		// Scene UUID
		auto sceneUUIDBytes = sceneUUID.as_bytes();
		writer.WriteRawBytes(reinterpret_cast<const uint8_t*>(sceneUUIDBytes.data()), 16);

		// Active camera UUID
		if (activeCamera && m_NodeUUIDs.count(activeCamera))
		{
			auto bytes = m_NodeUUIDs[activeCamera].as_bytes();
			writer.WriteRawBytes(reinterpret_cast<const uint8_t*>(bytes.data()), 16);
		}
		else
		{
			auto nullBytes = uuids::uuid{}.as_bytes();
			writer.WriteRawBytes(reinterpret_cast<const uint8_t*>(nullBytes.data()), 16);
		}

		// Node count (minus root)
		writer.WriteUInt32(static_cast<uint32_t>(m_NodeUUIDs.size() - 1));

		for (const auto& child : root->GetChildren())
			WriteNode(child.get(), uuids::uuid{}, writer);

		Core::Log::Info("BinarySceneWriter: Written binary scene: " + outputPath.string());
	}

	void BinarySceneWriter::AssignNodeUUIDs(Core::SceneObject* object, bool isRoot)
	{
		if (!object)
			return;

		m_NodeUUIDs[object] = GenerateUUID();

		if (!isRoot && object->HasSourceScene())
			return;

		for (const auto& child : object->GetChildren())
			AssignNodeUUIDs(child.get());
	}

	void BinarySceneWriter::WriteNode(Core::SceneObject* object, const uuids::uuid& parentUUID, BinaryWriter& writer)
	{
		if (!object)
			return;

		// Node UUID
		auto nodeUUIDBytes = m_NodeUUIDs[object].as_bytes();
		writer.WriteRawBytes(reinterpret_cast<const uint8_t*>(nodeUUIDBytes.data()), 16);

		// Parent UUID
		auto parentUUIDBytes = parentUUID.as_bytes();
		writer.WriteRawBytes(reinterpret_cast<const uint8_t*>(parentUUIDBytes.data()), 16);
		
		if (object->HasSourceScene())
		{
			// Has Scene UUID (true)
			writer.WriteUInt8(1);
			// Scene UUID
			auto uuidBytes = object->GetSourceSceneUUID().value().as_bytes();
			writer.WriteRawBytes(reinterpret_cast<const uint8_t*>(uuidBytes.data()), 16);
		}
		else
		{
			// Has Scene UUID (false)
			writer.WriteUInt8(0);
		}

		// Type name
		const char* typeName = object->GetTypeInfo()->name;
		uint16_t typeNameLength = static_cast<uint16_t>(std::strlen(typeName));
		writer.WriteUInt16(typeNameLength);
		writer.WriteRawBytes(reinterpret_cast<const uint8_t*>(typeName), typeNameLength);

		// Field count
		uint16_t fieldCount = 0;
		for (const TypeInfo* type = object->GetTypeInfo(); type; type = type->parent)
			fieldCount += static_cast<uint16_t>(type->fieldCount);
		writer.WriteUInt16(fieldCount);

		// Fields
		WriteFields(static_cast<void*>(object), object->GetTypeInfo(), writer);

		// Do not serialize children of scene instance
		if (object->HasSourceScene())
			return;

		for (const auto& child : object->GetChildren())
			WriteNode(child.get(), m_NodeUUIDs[object], writer);
	}

	void BinarySceneWriter::WriteFields(void* object, const TypeInfo* type, BinaryWriter& writer)
	{
		if (!object || !type)
			return;

		if (type->parent)
			WriteFields(object, type->parent, writer);

		if (!type->HasFields())
			return;

		for (const FieldInfo* field = type->Begin(); field != type->End(); ++field)
		{
			writer.WriteUInt32(field->nameHash);

			switch (field->kind)
			{
			case FieldKind::Bool:
			{
				writer.WriteUInt16(sizeof(uint8_t));
				writer.WriteUInt8(*field->GetPtrAs<bool>(object) ? 1 : 0);
				break;
			}
			case FieldKind::Int32:
			{
				writer.WriteUInt16(sizeof(int32_t));
				writer.WriteInt32(*field->GetPtrAs<int32_t>(object));
				break;
			}
			case FieldKind::UInt32:
			{
				writer.WriteUInt16(sizeof(uint32_t));
				writer.WriteUInt32(*field->GetPtrAs<uint32_t>(object));
				break;
			}
			case FieldKind::Float:
			{
				writer.WriteUInt16(sizeof(float));
				writer.WriteUInt32(*field->GetPtrAs<uint32_t>(object));
				break;
			}
			case FieldKind::String:
			{
				const std::string& string = *field->GetPtrAs<std::string>(object);
				uint32_t length = static_cast<uint32_t>(string.size());
				writer.WriteUInt16(static_cast<uint16_t>(sizeof(uint32_t) + length));
				writer.WriteUInt32(length);
				writer.WriteRawBytes(reinterpret_cast<const uint8_t*>(string.data()), length);
				break;
			}
			case FieldKind::Vector2:
			{
				writer.WriteUInt16(sizeof(float) * 2);
				auto* vec = field->GetPtrAs<glm::vec2>(object);
				writer.WriteFloat(vec->x);
				writer.WriteFloat(vec->y);
				break;
			}
			case FieldKind::Vector3:
			{
				writer.WriteUInt16(sizeof(float) * 3);
				auto* vec = field->GetPtrAs<glm::vec3>(object);
				writer.WriteFloat(vec->x);
				writer.WriteFloat(vec->y);
				writer.WriteFloat(vec->z);
				break;
			}
			case FieldKind::Vector4:
			{
				writer.WriteUInt16(sizeof(float) * 4);
				auto* vec = field->GetPtrAs<glm::vec4>(object);
				writer.WriteFloat(vec->x);
				writer.WriteFloat(vec->y);
				writer.WriteFloat(vec->z);
				writer.WriteFloat(vec->w);
				break;
			}
			case FieldKind::Quat:
			{
				writer.WriteUInt16(sizeof(float) * 4);
				auto* quat = field->GetPtrAs<glm::quat>(object);
				writer.WriteFloat(quat->x);
				writer.WriteFloat(quat->y);
				writer.WriteFloat(quat->z);
				writer.WriteFloat(quat->w);
				break;
			}
			case FieldKind::UUID:
			{
				writer.WriteUInt16(16);
				auto bytes = field->GetPtrAs<uuids::uuid>(object)->as_bytes();
				writer.WriteRawBytes(reinterpret_cast<const uint8_t*>(bytes.data()), 16);
				break;
			}
			case FieldKind::AssetRef:
			{
				writer.WriteUInt16(16);
				auto bytes = field->GetPtrAs<uuids::uuid>(object)->as_bytes();
				writer.WriteRawBytes(reinterpret_cast<const uint8_t*>(bytes.data()), 16);
				break;
			}
			case FieldKind::Object:
			{
				writer.WriteUInt16(0);
				if (field->type)
					WriteFields(field->GetPtr(object), field->type, writer);
				break;
			}
			case FieldKind::Unknown:
				Core::Log::Info("BinarySceneWriter: Unknown FieldKind");
				break;
			default:
				Core::Log::Info("BinarySceneWriter: Unhandled FieldKind: " + std::to_string(static_cast<uint8_t>(field->kind)));
				break;
			}
		}
	}
	
	uuids::uuid BinarySceneWriter::GenerateUUID() const
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
