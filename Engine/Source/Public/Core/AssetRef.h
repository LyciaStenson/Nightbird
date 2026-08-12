#pragma once

#include "Core/AssetManager.h"

#include <uuid.h>

#include <memory>

namespace Nightbird::Core
{
	template<typename T>
	class AssetRef
	{
	public:
		const uuids::uuid& GetUUID() const { return m_UUID; }

		bool IsValid() const { return m_Asset != nullptr; }

		std::shared_ptr<T> Get() const { return m_Asset; }

		explicit operator bool() const { return IsValid(); }

		void Resolve(std::shared_ptr<T> asset) { m_Asset = asset; }
		void SetUUID(const uuids::uuid& uuid) { m_UUID = uuid; }

	private:
		// UUID must be first member for reflection
		uuids::uuid m_UUID;
		std::shared_ptr<T> m_Asset;
	};
}
