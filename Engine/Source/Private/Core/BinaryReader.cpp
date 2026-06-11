#include "Core/BinaryReader.h"

#include "Core/Vertex.h"

#include <cstring>

namespace Nightbird::Core
{
	BinaryReader::BinaryReader(const std::string& path)
		: m_File(path, std::ios::binary | std::ios::in)
	{

	}

	bool BinaryReader::IsValid() const
	{
		return m_File.is_open() && m_File.good();
	}

	template<typename T>
	T BinaryReader::ReadValue()
	{
		uint8_t buffer[sizeof(T)];
		m_File.read(reinterpret_cast<char*>(buffer), sizeof(T));
		T value;
		std::memcpy(&value, buffer, sizeof(T));
		return value;
	}

	uint8_t BinaryReader::ReadUInt8()
	{
		uint8_t value;
		m_File.read(reinterpret_cast<char*>(&value), 1);
		return value;
	}

	uint16_t BinaryReader::ReadUInt16()
	{
		return ReadValue<uint16_t>();
	}

	uint32_t BinaryReader::ReadUInt32()
	{
		return ReadValue<uint32_t>();
	}

	int8_t BinaryReader::ReadInt8()
	{
		int8_t value;
		m_File.read(reinterpret_cast<char*>(&value), 1);
		return value;
	}

	int16_t BinaryReader::ReadInt16()
	{
		return ReadValue<int16_t>();
	}

	int32_t BinaryReader::ReadInt32()
	{
		return ReadValue<int32_t>();
	}

	float BinaryReader::ReadFloat()
	{
		return ReadValue<float>();
	}

	Vertex BinaryReader::ReadVertex()
	{
		Vertex vertex;
		vertex.position.x = ReadFloat();
		vertex.position.y = ReadFloat();
		vertex.position.z = ReadFloat();
		vertex.normal.x = ReadFloat();
		vertex.normal.y = ReadFloat();
		vertex.normal.z = ReadFloat();
		vertex.baseColorTexCoord.x = ReadFloat();
		vertex.baseColorTexCoord.y = ReadFloat();
		vertex.metallicRoughnessTexCoord.x = ReadFloat();
		vertex.metallicRoughnessTexCoord.y = ReadFloat();
		vertex.normalTexCoord.x = ReadFloat();
		vertex.normalTexCoord.y = ReadFloat();
		return vertex;
	}
	
	std::vector<Vertex> BinaryReader::ReadVertices(uint32_t count)
	{
		std::vector<Vertex> vertices(count);
		for (auto& vertex : vertices)
			vertex = ReadVertex();
		return vertices;
	}
	
	std::vector<uint16_t> BinaryReader::ReadIndices(uint32_t count)
	{
		std::vector<uint16_t> indices(count);
		for (auto& index : indices)
			index = ReadUInt16();
		return indices;
	}

	void BinaryReader::ReadRawBytes(uint8_t* destination, size_t size)
	{
		m_File.read(reinterpret_cast<char*>(destination), size);
	}
}
