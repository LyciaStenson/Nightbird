#include "Import/TextCubemapWriter.h"

#include "Import/AssetInfo.h"

#include "Core/Cubemap.h"
#include "Core/Log.h"

#include <toml.hpp>

namespace Nightbird::Editor
{
	static constexpr const char* s_FaceKeys[6] = {"pos_x", "neg_x", "pos_y", "neg_y", "pos_z", "neg_z"};
	
	void TextCubemapWriter::Write(const AssetInfo& assetInfo, const std::filesystem::path& outputPath)
	{
		toml::table document;

		toml::table cubemapTable;
		cubemapTable.insert("uuid", uuids::to_string(assetInfo.uuid));
		document.insert("cubemap", cubemapTable);
		
		toml::table facesTable;
		for (const char* key : s_FaceKeys)
		{
			auto it = assetInfo.tags.find(key);
			facesTable.insert(key, it != assetInfo.tags.end() ? it->second : std::string{});
		}
		document.insert("faces", facesTable);
		
		std::filesystem::create_directories(outputPath.parent_path());
		std::ofstream file(outputPath);
		file << document;
		file.close();

		Core::Log::Info("TextCubemapWriter: Written .ntcubemap: " + outputPath.string());
	}
}