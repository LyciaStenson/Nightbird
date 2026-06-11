#pragma once

#include "Core/SceneReadResult.h"
#include "Core/TypeInfo.h"

#include <uuid.h>

#include <memory>
#include <string>

namespace Nightbird::Core
{
	class Camera;
	struct Transform;
	
	class BinaryReader;

	class BinarySceneReader
	{
	public:
		SceneReadResult Read(const std::string& cookedDir, const uuids::uuid& uuid);

	private:
		void ReadFields(uint8_t* object, const TypeInfo* type, BinaryReader& reader);
		void ReadField(uint8_t* object, const TypeInfo* type, uint32_t nameHash, uint16_t size, BinaryReader& reader);
		void SkipFields(uint16_t fieldCount, BinaryReader& reader);
	};
}
