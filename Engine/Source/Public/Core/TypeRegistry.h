#pragma once

#include "Core/TypeInfo.h"

#include <vector>
#include <string_view>

namespace Nightbird
{
	using RegisterTypeFn = void(*)(TypeInfo* type);
	using ProjectInitFn = void(*)(RegisterTypeFn registerType);

	using ReflectionApplyFn = void(*)();
	void RegisterReflectionApply(ReflectionApplyFn fn) noexcept;

	class TypeRegistry
	{
	public:
		static void InitReflection() noexcept;

		static void Register(TypeInfo* type) noexcept;

		static const TypeInfo* Find(std::string_view name) noexcept;
		static const TypeInfo* Find(uint32_t hash) noexcept;

		static const std::vector<TypeInfo*>& GetAll() noexcept;
	};
}