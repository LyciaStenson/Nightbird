#pragma once

#include "Import/SceneImporter.h"

namespace fastgltf
{
	class Asset;
	struct Mesh;
	struct Image;
}

namespace Nightbird::Core
{
	class SceneObject;
	class Mesh;
	struct Material;
	class Texture;
}

namespace Nightbird::Editor
{
	class GltfSceneImporter : public SceneImporter
	{
	public:
		GltfSceneImporter() = default;

		std::string GetName() const override { return "gltf_scene"; }
		bool SupportsExtension(const std::string& extension) const override;
		Core::SceneReadResult Load(const AssetInfo& assetInfo, Core::AssetManager* assetManager) override;

	private:
		void ProcessNode(const fastgltf::Asset& gltfAsset, size_t nodeIndex, Core::SceneObject* parent, const std::vector<std::shared_ptr<Core::Material>>& materials, Core::AssetManager* assetManager);

		std::vector<std::shared_ptr<Core::Texture>> LoadTextures(const fastgltf::Asset& gltfAsset);
		std::vector<std::shared_ptr<Core::Material>> LoadMaterials(const fastgltf::Asset& gltfAsset, const std::vector<std::shared_ptr<Core::Texture>>& textures);
		std::shared_ptr<Core::Mesh> LoadMesh(const fastgltf::Asset& gltfAsset, const fastgltf::Mesh& gltfMesh, const std::vector<std::shared_ptr<Core::Material>>& materials);

		bool DecodeImage(const fastgltf::Asset& gltfAsset, const fastgltf::Image& image, std::vector<uint8_t>& outPixels, int& outWidth, int& outHeight);

		uuids::uuid GenerateUUID() const;
	};
}
