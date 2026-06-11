#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <cstdint>

namespace Nightbird::Core
{
	struct Vertex;

	class BinaryReader
	{
	public:
		BinaryReader(const std::string& path);
		bool IsValid() const;

		uint8_t ReadUInt8();
		uint16_t ReadUInt16();
		uint32_t ReadUInt32();
		int8_t ReadInt8();
		int16_t ReadInt16();
		int32_t ReadInt32();
		float ReadFloat();

		Vertex ReadVertex();
		std::vector<Vertex> ReadVertices(uint32_t count);
		std::vector<uint16_t> ReadIndices(uint32_t count);
		void ReadRawBytes(uint8_t* destination, size_t size);

	private:
		std::ifstream m_File;

		template<typename T>
		T ReadValue();
	};
}
