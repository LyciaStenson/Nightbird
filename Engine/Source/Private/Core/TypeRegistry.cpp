#include "Core/TypeRegistry.h"

#include <Core/Log.h>

#include <unordered_map>
#include <cassert>

namespace Nightbird
{
	static std::vector<ReflectionApplyFn>& GetReflectionApplyList() noexcept
	{
		static std::vector<ReflectionApplyFn> s_List;
		return s_List;
	}

	void RegisterReflectionApply(ReflectionApplyFn fn) noexcept
	{
		GetReflectionApplyList().push_back(fn);
	}

	static std::vector<TypeInfo*>& GetTypes() noexcept
	{
		static std::vector<TypeInfo*> s_Types;
		return s_Types;
	}

	static std::unordered_map<uint32_t, TypeInfo*>& GetMap() noexcept
	{
		static std::unordered_map<uint32_t, TypeInfo*> s_Map;
		return s_Map;
	}

	void TypeRegistry::InitReflection() noexcept
	{
		auto& list = GetReflectionApplyList();
		for (auto apply : list)
		{
			apply();
		}
	}
	
	void TypeRegistry::Register(TypeInfo* type) noexcept
	{
		if (!type)
			return;

		Core::Log::Info("TypeRegistry::Register: type: " + std::string(type->name) + ", hash: " + std::to_string(type->nameHash));

		auto& map = GetMap();
		auto [it, inserted] = map.emplace(type->nameHash, type);

		if (!inserted)
		{
			//if (it->second != type)
				//Core::Log::Error("TypeRegistry: Hash collision or duplicate name: " + std::string(type->name));
			//assert(it->second == type && "TypeRegistry: Hash collision or duplicate name");
			return;
		}
		
		GetTypes().push_back(type);
	}

	const TypeInfo* TypeRegistry::Find(std::string_view name) noexcept
	{
		return Find(FNVHash(name));
	}

	const TypeInfo* TypeRegistry::Find(uint32_t hash) noexcept
	{
		auto& map = GetMap();
		auto it = map.find(hash);
		return it != map.end() ? it->second : nullptr;
	}

	const std::vector<TypeInfo*>& TypeRegistry::GetAll() noexcept
	{
		return GetTypes();
	}
}