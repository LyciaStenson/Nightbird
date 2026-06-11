#pragma once

#include "Core/SceneReadResult.h"

#include <uuid.h>

#include <memory>

namespace Nightbird::Core
{
	class Mesh;
	struct Material;
	class Texture;
	class Cubemap;
	class AudioAsset;

	class AssetManager
	{
	public:
		virtual ~AssetManager() = default;
		
		template<typename T>
		std::weak_ptr<T> Load(const uuids::uuid& uuid)
		{
			auto it = m_Cache.find(uuid);
			if (it != m_Cache.end())
				return std::static_pointer_cast<T>(it->second);

			std::shared_ptr<T> asset = LoadInternal<T>(uuid);
			if (asset)
				m_Cache[uuid] = asset;
			return asset;
		}

		template<typename T>
		void Insert(const uuids::uuid& uuid, std::shared_ptr<T> asset)
		{
			m_Cache[uuid] = asset;
		}

		void Unload(const uuids::uuid& uuid)
		{
			m_Cache.erase(uuid);
		}

		void UnloadAll()
		{
			m_Cache.clear();
		}

	protected:
		virtual SceneReadResult LoadScene(const uuids::uuid& uuid) = 0;
		virtual std::shared_ptr<Mesh> LoadMesh(const uuids::uuid& uuid) = 0;
		virtual std::shared_ptr<Material> LoadMaterial(const uuids::uuid& uuid) = 0;
		virtual std::shared_ptr<Texture> LoadTexture(const uuids::uuid& uuid) = 0;
		virtual std::shared_ptr<Cubemap> LoadCubemap(const uuids::uuid& uuid) = 0;
		virtual std::shared_ptr<AudioAsset> LoadAudio(const uuids::uuid& uuid) = 0;

	private:
		template<typename T>
		std::shared_ptr<T> LoadInternal(const uuids::uuid& uuid)
		{
			if constexpr (std::is_same_v<T, Mesh>)
				return LoadMesh(uuid);
			if constexpr (std::is_same_v <T, Material>)
				return LoadMaterial(uuid);
			if constexpr (std::is_same_v <T, Texture>)
				return LoadTexture(uuid);
			if constexpr (std::is_same_v <T, Cubemap>)
				return LoadCubemap(uuid);
			if constexpr (std::is_same_v<T, AudioAsset>)
				return LoadAudio(uuid);
			return nullptr;
		}
		
		std::unordered_map<uuids::uuid, std::shared_ptr<void>> m_Cache;
	};
}