#include "Cook/ProjectCooker.h"

#include "Cook/BinaryWriter.h"

#include "Core/Log.h"

namespace Nightbird::Editor
{
	void ProjectCooker::Cook(const uuids::uuid& mainSceneUUID, const std::filesystem::path& outputDir, Endianness endianness)
	{
		std::filesystem::create_directories(outputDir);
		std::filesystem::path outputPath = outputDir / "Project.nbproject";
		BinaryWriter writer(outputPath, endianness);

		// Type signature
		writer.WriteUInt8('P');
		writer.WriteUInt8('R');
		writer.WriteUInt8('O');
		writer.WriteUInt8('J');

		// Version
		writer.WriteUInt32(1);

		auto bytes = mainSceneUUID.as_bytes();
		writer.WriteRawBytes(reinterpret_cast<const uint8_t*>(bytes.data()), 16);

		Core::Log::Info("ProjectCooker: Written .nbproject: " + outputPath.string());
	}
}
