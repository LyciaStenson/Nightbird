#include "Core/TypeInfo.h"

namespace Nightbird
{
	const FieldInfo* TypeInfo::Begin() const noexcept
	{
		return fields;
	}

	const FieldInfo* TypeInfo::End() const noexcept
	{
		return fields + fieldCount;
	}
}
