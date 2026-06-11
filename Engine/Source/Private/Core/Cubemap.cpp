#include "Core/Cubemap.h"

NB_REFLECT_NO_FIELDS(Nightbird::Core::Cubemap, NB_NO_PARENT, NB_NO_FACTORY)

namespace Nightbird::Core
{
	Cubemap::Cubemap(uint32_t faceSize, std::vector<uint8_t> data)
		: m_FaceSize(faceSize), m_Data(std::move(data))
	{

	}

	uint32_t Cubemap::GetFaceSize() const
	{
		return m_FaceSize;
	}

	const std::vector<uint8_t>& Cubemap::GetData() const
	{
		return m_Data;
	}

	void Cubemap::DiscardData()
	{
		m_Data.clear();
		m_Data.shrink_to_fit();
	}

	bool Cubemap::HasData() const
	{
		return !m_Data.empty();
	}
}