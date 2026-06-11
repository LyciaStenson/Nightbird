#include "Import/ImportManager.h"

#include "Import/AssetInfo.h"
#include "Import/TextSceneImporter.h"
#include "Import/TextCubemapImporter.h"

#include "Import/GltfSceneImporter.h"
#include "Import/StbTextureImporter.h"
#include "Import/DrLibsAudioImporter.h"

#include "Core/AssetManager.h"
#include "Core/Log.h"

#include <toml.hpp>

#include <random>

namespace Nightbird::Editor
{
	ImportManager::ImportManager(const std::filesystem::path& assetsDir)
		: m_AssetsDir(assetsDir)
	{
		m_Importers.push_back(std::make_unique<TextSceneImporter>());
		m_Importers.push_back(std::make_unique<GltfSceneImporter>());
		m_Importers.push_back(std::make_unique<TextCubemapImporter>());
		m_Importers.push_back(std::make_unique<StbTextureImporter>());
		m_Importers.push_back(std::make_unique<DrLibsAudioImporter>());
	}

	void ImportManager::Scan()
	{
		if (!std::filesystem::exists(m_AssetsDir))
		{
			Core::Log::Warning("Assets directory does not exist: " + m_AssetsDir.string());
			return;
		}

		for (const auto& entry : std::filesystem::recursive_directory_iterator(m_AssetsDir))
		{
			if (!entry.is_regular_file())
				continue;

			if (entry.path().extension() == ".assetinfo")
				continue;

			for (const auto& importer : m_Importers)
			{
				if (!importer->SupportsExtension(entry.path().extension().string()))
					continue;

				if (importer->HasEmbeddedAssetInfo())
				{
					auto assetInfo = importer->ReadEmbeddedAssetInfo(entry.path());
					if (assetInfo)
						m_AssetInfos[assetInfo->uuid] = std::move(*assetInfo);
				}
				else
				{
					std::filesystem::path assetInfoPath = entry.path().string() + ".assetinfo";
					if (!std::filesystem::exists(assetInfoPath))
						GenerateAssetInfoFile(entry.path());
					else
						ReadAssetInfoFile(assetInfoPath);
				}
			}
		}
	}

	const std::filesystem::path& ImportManager::GetAssetsDir() const
	{
		return m_AssetsDir;
	}

	const std::unordered_map<uuids::uuid, AssetInfo>& ImportManager::GetAssetInfos() const
	{
		return m_AssetInfos;
	}

	const AssetInfo* ImportManager::GetAssetInfo(const uuids::uuid& uuid) const
	{
		auto it = m_AssetInfos.find(uuid);
		if (it != m_AssetInfos.end())
			return &it->second;
		return nullptr;
	}

	const AssetInfo* ImportManager::GetAssetInfo(const std::filesystem::path& path) const
	{
		for (const auto& [uuid, assetInfo] : m_AssetInfos)
		{
			if (assetInfo.path == path)
				return &assetInfo;
		}
		return nullptr;
	}

	AssetInfo* ImportManager::GetAssetInfo(const uuids::uuid& uuid)
	{
		auto it = m_AssetInfos.find(uuid);
		if (it != m_AssetInfos.end())
			return &it->second;
		return nullptr;
	}

	AssetInfo* ImportManager::GetAssetInfo(const std::filesystem::path& path)
	{
		for (auto& [uuid, assetInfo] : m_AssetInfos)
		{
			if (assetInfo.path == path)
				return &assetInfo;
		}
		return nullptr;
	}

	Importer* ImportManager::FindImporter(const std::filesystem::path& path) const
	{
		for (const auto& importer : m_Importers)
		{
			if (importer->SupportsExtension(path.extension().string()))
				return importer.get();
		}
		return nullptr;
	}

	void ImportManager::Register(AssetInfo assetInfo)
	{
		uuids::uuid uuid = assetInfo.uuid;
		m_AssetInfos[uuid] = std::move(assetInfo);
	}

	std::shared_ptr<Core::Mesh> ImportManager::LoadMesh(const uuids::uuid& uuid)
	{
		Core::Log::Warning("ImportManager: Individual mesh loading not yet supported");
		return nullptr;
	}

	std::shared_ptr<Core::Material> ImportManager::LoadMaterial(const uuids::uuid& uuid)
	{
		Core::Log::Warning("ImportManager: Individual material loading not yet supported");
		return nullptr;
	}

	std::shared_ptr<Core::Texture> ImportManager::LoadTexture(const uuids::uuid& uuid)
	{
		const AssetInfo* assetInfo = GetAssetInfo(uuid);
		if (!assetInfo)
		{
			Core::Log::Warning("ImportManager: AssetInfo not found for texture UUID: " + uuids::to_string(uuid));
			return nullptr;
		}

		for (const auto& importer : m_Importers)
		{
			if (importer->GetName() == assetInfo->importer)
			{
				if (auto* textureImporter = importer->AsTextureImporter())
					return textureImporter->Load(*assetInfo);
			}
		}

		Core::Log::Warning("ImportManager: No texture importer found for: " + assetInfo->importer);
		return nullptr;
	}

	std::shared_ptr<Core::Cubemap> ImportManager::LoadCubemap(const uuids::uuid& uuid)
	{
		const AssetInfo* assetInfo = GetAssetInfo(uuid);
		if (!assetInfo)
		{
			Core::Log::Warning("ImportManager: AssetInfo not found for cubemap UUID: " + uuids::to_string(uuid));
			return nullptr;
		}

		for (const auto& importer : m_Importers)
		{
			if (importer->GetName() == assetInfo->importer)
			{
				if (auto* cubemapImporter = importer->AsCubemapImporter())
				{
					CubemapReadResult result = cubemapImporter->Load(*assetInfo, this);
					return result.cubemap;
				}
			}
		}

		Core::Log::Warning("ImportManager: No cubemap importer found for: " + assetInfo->importer);
		return nullptr;
	}

	std::shared_ptr<Core::AudioAsset> ImportManager::LoadAudio(const uuids::uuid& uuid)
	{
		const AssetInfo* assetInfo = GetAssetInfo(uuid);
		if (!assetInfo)
		{
			Core::Log::Warning("ImportManager: AssetInfo not found for audio UUID: " + uuids::to_string(uuid));
			return nullptr;
		}

		for (const auto& importer : m_Importers)
		{
			if (importer->GetName() == assetInfo->importer)
			{
				if (auto* audioImporter = importer->AsAudioImporter())
					return audioImporter->Load(*assetInfo);
			}
		}

		Core::Log::Warning("ImportManager: No audio importer found for: " + assetInfo->importer);
		return nullptr;
	}

	Core::SceneReadResult ImportManager::LoadScene(const uuids::uuid& uuid)
	{
		Core::SceneReadResult result;

		const AssetInfo* assetInfo = GetAssetInfo(uuid);
		if (!assetInfo)
		{
			Core::Log::Warning("ImportManager: AssetInfo not found for scene UUID: " + uuids::to_string(uuid));
			return result;
		}

		for (const auto& importer : m_Importers)
		{
			if (importer->GetName() == assetInfo->importer)
			{
				if (auto* sceneImporter = importer->AsSceneImporter())
				{
					result = sceneImporter->Load(*assetInfo, this);
					break;
				}
			}
		}

		if (!result.root)
		{
			Core::Log::Warning("ImportManager: No scene importer found for: " + assetInfo->importer);
			return result;
		}

		for (auto& object : result.root->GetChildren())
		{
			if (const auto& sceneUUID = object->GetSourceSceneUUID())
			{
				Core::SceneReadResult nestedResult = LoadScene(*sceneUUID);
				if (nestedResult.root)
				{
					for (auto& nestedChild : nestedResult.root->GetChildren())
						object->AddChild(std::move(nestedChild));
				}
				else
				{
					Core::Log::Warning("ImportManager: Failed to load nested scene with UUID: " + uuids::to_string(*sceneUUID));
				}
			}
		}

		return result;
	}
	
	uuids::uuid ImportManager::GenerateUUID() const
	{
		std::random_device randomDevice;

		auto seedData = std::array<int, std::mt19937::state_size>{};
		std::generate(std::begin(seedData), std::end(seedData), std::ref(randomDevice));
		std::seed_seq seq(std::begin(seedData), std::end(seedData));
		std::mt19937 generator(seq);
		uuids::uuid_random_generator gen{generator};

		return gen();
	}

	std::string ImportManager::FindImporterName(const std::filesystem::path& assetPath)
	{
		for (const auto& importer : m_Importers)
		{
			if (importer->SupportsExtension(assetPath.extension().string()))
				return importer->GetName();
		}
		return {};
	}

	void ImportManager::GenerateAssetInfoFile(const std::filesystem::path& assetPath)
	{
		std::string importerName = FindImporterName(assetPath);
		if (importerName.empty())
			return;

		uuids::uuid uuid = GenerateUUID();

		toml::table info;
		info.insert("uuid", uuids::to_string(uuid));
		info.insert("importer", importerName);
		info.insert("importer_version", 1);

		toml::table table;
		table.insert("info", info);

		std::filesystem::path assetInfoPath = assetPath.string() + ".assetinfo";
		std::ofstream file(assetInfoPath);
		file << table;
		file.close();

		Core::Log::Info("Generated .assetinfo: " + assetInfoPath.string());

		ReadAssetInfoFile(assetInfoPath);
	}

	void ImportManager::ReadAssetInfoFile(const std::filesystem::path& assetInfoPath)
	{
		if (!std::filesystem::exists(assetInfoPath) || std::filesystem::file_size(assetInfoPath) == 0)
		{
			Core::Log::Error("Invalid .assetinfo: " + assetInfoPath.string());
			return;
		}

		toml::parse_result result = toml::parse_file(assetInfoPath.string());
		if (!result)
		{
			Core::Log::Error("Failed to parse .assetinfo: " + assetInfoPath.string());
			return;
		}
		
		toml::table table = result.table();
		
		std::string uuidString = table["info"]["uuid"].value_or(std::string{});
		auto uuid = uuids::uuid::from_string(uuidString);
		if (!uuid)
		{
			Core::Log::Warning("Invalid UUID in: " + assetInfoPath.string());
			return;
		}

		AssetInfo assetInfo;
		assetInfo.uuid = *uuid;
		assetInfo.importer = table["info"]["importer"].value_or(std::string{});
		
		std::string assetInfoPathString = assetInfoPath.string();
		assetInfo.path = assetInfoPathString.substr(0, assetInfoPathString.size() - std::string(".assetinfo").size());
		
		m_AssetInfos[assetInfo.uuid] = std::move(assetInfo);
	}
}
