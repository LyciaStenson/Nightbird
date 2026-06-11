#include "Core/BinarySceneReader.h"

#include "Core/BinaryReader.h"

#include "Core/Transform.h"
#include "Core/SceneObject.h"
#include "Core/SpatialObject.h"
#include "Core/MeshInstance.h"
#include "Core/DirectionalLight.h"
#include "Core/PointLight.h"
#include "Core/Camera.h"
#include "Core/AudioSource.h"
#include "Core/Log.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <vector>

namespace Nightbird::Core
{
	SceneReadResult BinarySceneReader::Read(const std::string& cookedDir, const uuids::uuid& uuid)
	{
		SceneReadResult result;
		result.root = std::make_unique<SceneObject>();

		std::string path = cookedDir + "/" + uuids::to_string(uuid) + ".nbscene";

		BinaryReader reader(path);
		if (!reader.IsValid())
		{
			Core::Log::Error("BinarySceneReader: Failed to open: " + path);
			return result;
		}
		
		// Validate type
		uint8_t signature[4] = {};
		reader.ReadRawBytes(signature, 4);
		if (signature[0] != 'S' || signature[1] != 'C' || signature[2] != 'N' || signature[3] != 'E')
		{
			Core::Log::Error("BinarySceneReader: Invalid type signature in: " + path);
			return result;
		}

		// Check Version
		uint32_t version = reader.ReadUInt32();
		if (version != 1)
		{
			Core::Log::Error("BinarySceneReader: Unsupported version: " + std::to_string(version));
			return result;
		}

		// Scene name
		uint32_t nameLength = reader.ReadUInt32();
		std::string sceneName(nameLength, '\0');
		reader.ReadRawBytes(reinterpret_cast<uint8_t*>(sceneName.data()), nameLength);
		result.root->SetName(sceneName);

		// Scene UUID
		std::array<uint8_t, 16> sceneUUIDBytes;
		reader.ReadRawBytes(sceneUUIDBytes.data(), 16);
		result.uuid = uuids::uuid(sceneUUIDBytes);

		// Active camera UUID
		std::array<uint8_t, 16> activeCameraUUIDBytes;
		reader.ReadRawBytes(activeCameraUUIDBytes.data(), 16);
		uuids::uuid activeCameraUUID(activeCameraUUIDBytes);

		// Node count
		uint32_t nodeCount = reader.ReadUInt32();

		std::unordered_map<uuids::uuid, Core::SceneObject*> nodeMap;
		std::unordered_map<uuids::uuid, std::unique_ptr<Core::SceneObject>> ownedNodeMap;
		std::unordered_map<uuids::uuid, uuids::uuid> parentMap;
		std::vector<uuids::uuid> nodeUUIDs;

		for (uint32_t i = 0; i < nodeCount; i++)
		{
			// Node UUID
			std::array<uint8_t, 16> nodeUUIDBytes;
			reader.ReadRawBytes(nodeUUIDBytes.data(), 16);
			uuids::uuid nodeUUID(nodeUUIDBytes);

			// Parent UUID
			std::array<uint8_t, 16> parentUUIDBytes;
			reader.ReadRawBytes(parentUUIDBytes.data(), 16);
			uuids::uuid parentUUID(parentUUIDBytes);

			// Source scene flag and UUID
			uint8_t hasSceneUUID = reader.ReadUInt8();
			uuids::uuid sourceSceneUUID;
			if (hasSceneUUID)
			{
				std::array<uint8_t, 16> sceneUUIDBytes;
				reader.ReadRawBytes(sceneUUIDBytes.data(), 16);
				sourceSceneUUID = uuids::uuid(sceneUUIDBytes);
			}

			// Type Name
			uint16_t typeNameLength = reader.ReadUInt16();
			std::string typeName(typeNameLength, '\0');
			reader.ReadRawBytes(reinterpret_cast<uint8_t*>(typeName.data()), typeNameLength);

			// Field count
			uint16_t fieldCount = reader.ReadUInt16();

			// Construct object via TypeRegistry
			std::unique_ptr<Core::SceneObject> object;
			const TypeInfo* type = TypeRegistry::Find(typeName);
			
			if (type && type->factory)
			{
				object.reset(type->CreateAs<SceneObject>());
				object->SetName(typeName);
				ReadFields(reinterpret_cast<uint8_t*>(object.get()), type, reader);
			}
			else
			{
				Log::Warning("BinarySceneReader: Unknown type " + typeName + ", defaulting to SceneObject");
				object = std::make_unique<SceneObject>();
				SkipFields(fieldCount, reader);

				Log::Info("BinarySceneReader: Available types are: ");
				for (const auto* type : TypeRegistry::GetAll())
				{
					Log::Info("BinarySceneReader: Available type: " + std::string(type->name));
				}
			}

			if (hasSceneUUID)
				object->SetSourceSceneUUID(sourceSceneUUID);

			if (nodeUUID == activeCameraUUID)
				result.activeCamera = Cast<Camera>(object.get());

			SceneObject* objectPtr = object.get();
			nodeMap[nodeUUID] = objectPtr;
			ownedNodeMap[nodeUUID] = std::move(object);
			nodeUUIDs.push_back(nodeUUID);

			if (!parentUUID.is_nil())
				parentMap[nodeUUID] = parentUUID;
		}

		// Build scene hierarchy
		for (const auto& nodeUUID : nodeUUIDs)
		{
			if (!ownedNodeMap.count(nodeUUID))
				continue;

			auto parentIt = parentMap.find(nodeUUID);
			if (parentIt != parentMap.end() && nodeMap.count(parentIt->second))
				nodeMap[parentIt->second]->AddChild(std::move(ownedNodeMap[nodeUUID]));
			else
				result.root->AddChild(std::move(ownedNodeMap[nodeUUID]));
		}

		Core::Log::Info("BinarySceneReader: Loaded scene: " + path);
		return result;
	}

	void BinarySceneReader::ReadFields(uint8_t* object, const TypeInfo* type, BinaryReader& reader)
	{
		if (!object || !type)
			return;

		if (type->parent)
			ReadFields(object, type->parent, reader);

		if (!type->HasFields())
			return;

		for (uint32_t i = 0; i < type->fieldCount; ++i)
		{
			uint32_t nameHash = reader.ReadUInt32();
			uint16_t size = reader.ReadUInt16();
			ReadField(object, type, nameHash, size, reader);
		}
	}
	
	void BinarySceneReader::ReadField(uint8_t* object, const TypeInfo* type, uint32_t nameHash, uint16_t size, BinaryReader& reader)
	{
		for (const TypeInfo* t = type; t!= nullptr; t = t->parent)
		{
			for (uint16_t i = 0; i < t->fieldCount; ++i)
			{
				const FieldInfo& field = t->fields[i];
				if (field.nameHash != nameHash)
					continue;

				uint8_t* fieldPtr = object + field.offset;
				
				if (size == 0)
				{
					if (field.kind == FieldKind::Object && field.type)
						ReadFields(fieldPtr, field.type, reader);
					else
						Log::Warning("BinarySceneReader: Size 0 for non-object field with hash: " + std::to_string(nameHash));
					return;
				}

				switch (field.kind)
				{
				case FieldKind::Bool:
					*reinterpret_cast<bool*>(fieldPtr) = reader.ReadUInt8() != 0;
					break;
				case FieldKind::Int32:
					*reinterpret_cast<int32_t*>(fieldPtr) = reader.ReadInt32();
					break;
				case FieldKind::UInt32:
					*reinterpret_cast<uint32_t*>(fieldPtr) = reader.ReadUInt32();
					break;
				case FieldKind::Float:
					*reinterpret_cast<float*>(fieldPtr) = reader.ReadFloat();
					break;
				case FieldKind::String:
				{
					uint32_t length = reader.ReadUInt32();
					std::string string(length, '\0');
					reader.ReadRawBytes(reinterpret_cast<uint8_t*>(string.data()), length);
					*reinterpret_cast<std::string*>(fieldPtr) = std::move(string);
					break;
				}
				case FieldKind::Vector2:
				{
					auto* vec = reinterpret_cast<glm::vec2*>(fieldPtr);
					vec->x = reader.ReadFloat();
					vec->y = reader.ReadFloat();
					break;
				}
				case FieldKind::Vector3:
				{
					auto* vec = reinterpret_cast<glm::vec3*>(fieldPtr);
					vec->x = reader.ReadFloat();
					vec->y = reader.ReadFloat();
					vec->z = reader.ReadFloat();
					break;
				}
				case FieldKind::Vector4:
				{
					auto* vec = reinterpret_cast<glm::vec4*>(fieldPtr);
					vec->x = reader.ReadFloat();
					vec->y = reader.ReadFloat();
					vec->z = reader.ReadFloat();
					vec->w = reader.ReadFloat();
					break;
				}
				case FieldKind::Quat:
				{
					auto* quat = reinterpret_cast<glm::quat*>(fieldPtr);
					quat->x = reader.ReadFloat();
					quat->y = reader.ReadFloat();
					quat->z = reader.ReadFloat();
					quat->w = reader.ReadFloat();
					break;
				}
				case FieldKind::UUID:
				{
					std::array<uint8_t, 16> bytes;
					reader.ReadRawBytes(bytes.data(), 16);
					*reinterpret_cast<uuids::uuid*>(fieldPtr) = uuids::uuid(bytes);
					break;
				}
				case FieldKind::AssetRef:
				{
					std::array<uint8_t, 16> bytes;
					reader.ReadRawBytes(bytes.data(), 16);
					*reinterpret_cast<uuids::uuid*>(fieldPtr) = uuids::uuid(bytes);
					break;
				}
				default:
					Core::Log::Info("BinarySceneReader: Unhandled FieldKind:");
				case FieldKind::Unknown:
					Core::Log::Info("BinarySceneReader: Unknown FieldKind for hash: " + std::to_string(nameHash));
					if (size > 0)
					{
						std::vector<uint8_t> discard(size);
						reader.ReadRawBytes(discard.data(), size);
					}
					break;
				}
				return;
			}
		}

		// Mo matching FieldKind found
		if (size > 0)
		{
			Log::Warning("BinarySceneReader: No matching FieldKind found for hash: " + std::to_string(nameHash) + ", skipping " + std::to_string(size) + " bytes");
			std::vector<uint8_t> discard(size);
			reader.ReadRawBytes(discard.data(), size);
		}
		else
		{
			Log::Warning("BinarySceneReader: Unknown nested type with hash " + std::to_string(nameHash) + ", cannot skip safely");
		}
	}

	void BinarySceneReader::SkipFields(uint16_t fieldCount, BinaryReader& reader)
	{
		for (uint16_t i = 0; i < fieldCount; ++i)
		{
			reader.ReadUInt32(); // nameHash
			uint16_t size = reader.ReadUInt16();
			if (size > 0)
			{
				std::vector<uint8_t> discard(size);
				reader.ReadRawBytes(discard.data(), size);
			}
			else
			{
				Log::Warning("BinarySceneReader: Cannot safely skip unknown nested Object field");
			}
		}
	}
}
