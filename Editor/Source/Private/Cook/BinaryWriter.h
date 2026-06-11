#pragma once

#include "Cook/Endianness.h"

#include <vector>
#include <filesystem>
#include <fstream>

namespace Nightbird::Core
{
	struct Vertex;
}

namespace Nightbird::Editor
{
	class BinaryWriter
	{
	public:
		BinaryWriter(const std::filesystem::path& path, Endianness endianness);

		void WriteUInt8(uint8_t value);
		void WriteUInt16(uint16_t value);
		void WriteUInt32(uint32_t value);
		void WriteInt8(int8_t value);
		void WriteInt16(int16_t value);
		void WriteInt32(int32_t value);
		void WriteFloat(float value);
		void WriteVertex(const Core::Vertex& vertex);
		void WriteVertices(const std::vector<Core::Vertex>& vertices);
		void WriteIndices(const std::vector<uint16_t>& indices);
		void WriteRawBytes(const uint8_t* data, size_t size);

	private:
		std::ofstream m_File;
		Endianness m_Endianness;

		template<typename T>
		void WriteValue(T value);
	};
}
