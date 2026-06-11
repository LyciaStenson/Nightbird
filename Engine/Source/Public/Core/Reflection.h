#pragma once

#include "Core/TypeInfo.h"
#include "Core/TypeRegistry.h"
#include "Core/ReflectionInternal.h"

#define NB_TYPE_BASE() \
	public: \
		static ::Nightbird::TypeInfo s_TypeInfo; \
		virtual const ::Nightbird::TypeInfo* GetTypeInfo() const { return &s_TypeInfo; }

#define NB_TYPE() \
	public: \
		static ::Nightbird::TypeInfo s_TypeInfo; \
		const ::Nightbird::TypeInfo* GetTypeInfo() const override { return &s_TypeInfo; } \

#define NB_PARENT(T) &T::s_TypeInfo
#define NB_NO_PARENT nullptr

#define NB_FACTORY(T) ([]() -> void* { return new T(); })
#define NB_NO_FACTORY nullptr

#define NB_CONCAT_IMPL(a, b) a##b
#define NB_CONCAT(a, b) NB_CONCAT_IMPL(a, b)

#define NB_REFLECT(Type, Parent, Factory, ...) \
	::Nightbird::TypeInfo Type::s_TypeInfo = { \
		#Type, \
		::Nightbird::FNVHash(#Type), \
		Parent, \
		Factory, \
		nullptr, \
		0 \
	}; \
	namespace NB_CONCAT(NB_Reflection, Type) \
	{ \
		using _NB_CurrentType = Type; \
		static const ::Nightbird::FieldInfo _nb_fields[] = { __VA_ARGS__ }; \
		inline void _nb_ApplyReflection() \
		{ \
			Type::s_TypeInfo.fields = _nb_fields; \
			Type::s_TypeInfo.fieldCount = \
				static_cast<uint32_t>(std::size(_nb_fields)); \
			::Nightbird::TypeRegistry::Register(&Type::s_TypeInfo); \
		} \
		static const bool _nb_registration_apply = ([]() { \
			::Nightbird::RegisterReflectionApply(&_nb_ApplyReflection); \
			return true; \
		}()); \
	}

#define NB_REFLECT_NO_FIELDS(Type, Parent, Factory) \
	::Nightbird::TypeInfo Type::s_TypeInfo = { \
		#Type, \
		::Nightbird::FNVHash(#Type), \
		Parent, \
		Factory, \
		nullptr, \
		0 \
	}; \
	namespace NB_CONCAT(NB_Reflection, Type) \
	{ \
		inline void _nb_ApplyReflection() \
		{ \
			::Nightbird::TypeRegistry::Register(&Type::s_TypeInfo); \
		} \
		static const bool _nb_registration_apply = ([]() { \
			::Nightbird::RegisterReflectionApply(&_nb_ApplyReflection); \
			return true; \
		}()); \
	}

#define NB_FIELD(Name) \
	{ \
		#Name, \
		::Nightbird::FNVHash(#Name), \
		::Nightbird::Detail::GetFieldTypeInfo<std::decay_t<decltype(_NB_CurrentType::Name)>>(), \
		::Nightbird::Detail::DeduceFieldKind<std::decay_t<decltype(_NB_CurrentType::Name)>>(), \
		static_cast<uint32_t>(offsetof(_NB_CurrentType, Name)), \
		static_cast<uint32_t>(sizeof(_NB_CurrentType::Name)) \
	}
