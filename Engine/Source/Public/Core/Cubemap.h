#pragma once

#include "Core/Reflection.h"

#include <vector>

namespace Nightbird::Core
{
	class Cubemap
	{
	public:
		NB_TYPE_BASE()
		Cubemap(uint32_t faceSize, std::vector<uint8_t> data);

		uint32_t GetFaceSize() const;

		const std::vector<uint8_t>& GetData() const;
		void DiscardData();
		bool HasData() const;

	private:
		uint32_t m_FaceSize;
		std::vector<uint8_t> m_Data;
	};
}