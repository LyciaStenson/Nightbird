#pragma once

#include <cstdint>
#include <string_view>

namespace Nightbird
{
	struct TypeInfo;
	struct FieldInfo;

	// FNV-1a hash
	constexpr uint32_t FNVHash(std::string_view s) noexcept
	{
		uint32_t h = 2166136261u;
		for (char c : s)
			h = (h ^ static_cast<uint8_t>(c)) * 16777619u;
		return h;
	}
	
	struct TypeInfo
	{
		const char* name = nullptr;
		uint32_t nameHash = 0;

		const TypeInfo* parent = nullptr;

		using FactoryFn = void* (*)();
		FactoryFn factory = nullptr;

		const FieldInfo* fields = nullptr;
		uint32_t fieldCount = 0;

		bool IsA(const TypeInfo* other) const noexcept
		{
			for (auto* current = this; current; current = current->parent)
			{
				if (current->nameHash == other->nameHash)
					return true;
			}
			return false;
		}

		void* Create() const noexcept
		{
			return factory ? factory() : nullptr;
		}

		template<typename T>
		T* CreateAs() const noexcept
		{
			return static_cast<T*>(Create());
		}

		bool HasFactory() const noexcept
		{
			return factory != nullptr;
		}

		bool HasFields() const noexcept
		{
			return fields != nullptr && fieldCount > 0;
		}

		const FieldInfo* Begin() const noexcept;
		const FieldInfo* End() const noexcept;
	};

	enum class FieldKind : uint8_t
	{
		Bool, Int32, UInt32, Float,
		String,
		Vector2, Vector3, Vector4, Quat,
		UUID,
		AssetRef,
		Object, // Has TypeInfo
		Unknown
	};

	struct FieldInfo
	{
		const char* name = nullptr;
		uint32_t nameHash = 0;

		const TypeInfo* type = nullptr;
		FieldKind kind = FieldKind::Unknown;

		uint32_t offset = 0;
		uint32_t size = 0;

		void* GetPtr(void* object) const noexcept
		{
			return static_cast<char*>(object) + offset;
		}

		template<typename T>
		T* GetPtrAs(void* object) const noexcept
		{
			return static_cast<T*>(GetPtr(object));
		}
	};
	
	template<typename T, typename U>
	T* Cast(U* obj)
	{
		if (obj && obj->GetTypeInfo()->IsA(&T::s_TypeInfo))
			return static_cast<T*>(obj);
		return nullptr;
	}

	template<typename T, typename U>
	const T* Cast(const U* obj)
	{
		if (obj && obj->GetTypeInfo()->IsA(&T::s_TypeInfo))
			return static_cast<const T*>(obj);
		return nullptr;
	}

	template<typename T, typename U>
	bool IsA(const U* obj)
	{
		return obj && obj->GetTypeInfo()->IsA(&T::s_TypeInfo);
	}
}
