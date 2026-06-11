#pragma once

#include "Core/TypeInfo.h"
#include "Core/AssetRef.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <uuid.h>

namespace Nightbird::Detail
{
	template<typename T, typename = void>
	struct HasTypeInfo : std::false_type {};
	template<typename T>
	struct HasTypeInfo<T, std::void_t<decltype(T::s_TypeInfo)>> : std::true_type {};

	template<typename T>
	struct AssetRefInner {};
	template<typename T>
	struct AssetRefInner<Core::AssetRef<T>> { using Type = T; };

	template<typename T, typename = void>
	struct IsAssetRef : std::false_type {};

	template<typename T>
	struct IsAssetRef<Core::AssetRef<T>> : std::true_type {};

	template<typename T>
	const TypeInfo* GetFieldTypeInfo()
	{
		if constexpr (IsAssetRef<T>::value)
			return &AssetRefInner<T>::Type::s_TypeInfo;
		else if constexpr (HasTypeInfo<T>::value)
			return &T::s_TypeInfo;
		else
			return nullptr;
	}

	template<typename T>
	constexpr FieldKind DeduceFieldKind()
	{
		if constexpr (std::is_same_v<T, bool>)
			return FieldKind::Bool;
		else if constexpr (std::is_same_v<T, int32_t>)
			return FieldKind::Int32;
		else if constexpr (std::is_same_v<T, uint32_t>)
			return FieldKind::UInt32;
		else if constexpr (std::is_same_v<T, float>)
			return FieldKind::Float;
		else if constexpr (std::is_same_v<T, std::string>)
			return FieldKind::String;
		else if constexpr (std::is_same_v<T, glm::vec3>)
			return FieldKind::Vector3;
		else if constexpr (std::is_same_v<T, glm::vec4>)
			return FieldKind::Vector4;
		else if constexpr (std::is_same_v<T, glm::quat>)
			return FieldKind::Quat;
		else if constexpr (std::is_same_v<T, uuids::uuid>)
			return FieldKind::UUID;
		else if constexpr (IsAssetRef<T>::value)
			return FieldKind::AssetRef;
		else if constexpr (HasTypeInfo<T>::value)
			return FieldKind::Object;
		else
			return FieldKind::Unknown;
	}
}
