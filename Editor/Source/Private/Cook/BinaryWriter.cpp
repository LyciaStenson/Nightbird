#include "Cook/BinaryWriter.h"

#include "Core/Vertex.h"

#include <cstring>

namespace Nightbird::Editor
{
	BinaryWriter::BinaryWriter(const std::filesystem::path& path, Endianness endianness)
		: m_File(path, std::ios::binary | std::ios::out), m_Endianness(endianness)
	{

	}

	template<typename T>
	void BinaryWriter::WriteValue(T value)
	{
		uint8_t bytes[sizeof(T)];
		std::memcpy(bytes, &value, sizeof(T));

		if (m_Endianness == Endianness::Big)
		{
			for (size_t i = 0; i < sizeof(T) / 2; ++i)
				std::swap(bytes[i], bytes[sizeof(T) - 1 - i]);
		}

		m_File.write(reinterpret_cast<const char*>(bytes), sizeof(T));
	}

	void BinaryWriter::WriteUInt8(uint8_t value)
	{
		m_File.write(reinterpret_cast<const char*>(&value), 1);
	}

	void BinaryWriter::WriteUInt16(uint16_t value)
	{
		WriteValue(value);
	}

	void BinaryWriter::WriteUInt32(uint32_t value)
	{
		WriteValue(value);
	}

	void BinaryWriter::WriteInt8(int8_t value)
	{
		m_File.write(reinterpret_cast<const char*>(&value), 1);
	}

	void BinaryWriter::WriteInt16(int16_t value)
	{
		WriteValue(value);
	}

	void BinaryWriter::WriteInt32(int32_t value)
	{
		WriteValue(value);
	}

	void BinaryWriter::WriteFloat(float value)
	{
		WriteValue(value);
	}

	void BinaryWriter::WriteVertex(const Core::Vertex& vertex)
	{
		WriteFloat(vertex.position.x);
		WriteFloat(vertex.position.y);
		WriteFloat(vertex.position.z);
		WriteFloat(vertex.normal.x);
		WriteFloat(vertex.normal.y);
		WriteFloat(vertex.normal.z);
		WriteFloat(vertex.baseColorTexCoord.x);
		WriteFloat(vertex.baseColorTexCoord.y);
		WriteFloat(vertex.metallicRoughnessTexCoord.x);
		WriteFloat(vertex.metallicRoughnessTexCoord.y);
		WriteFloat(vertex.normalTexCoord.x);
		WriteFloat(vertex.normalTexCoord.y);
	}

	void BinaryWriter::WriteVertices(const std::vector<Core::Vertex>& vertices)
	{
		for (const auto& vertex : vertices)
			WriteVertex(vertex);
	}

	void BinaryWriter::WriteIndices(const std::vector<uint16_t>& indices)
	{
		for (uint16_t index : indices)
			WriteUInt16(index);
	}
	
	void BinaryWriter::WriteRawBytes(const uint8_t* data, size_t size)
	{
		m_File.write(reinterpret_cast<const char*>(data), size);
	}
}
