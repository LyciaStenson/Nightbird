#include "Import/StbTextureImporter.h"

#include "Core/Texture.h"
#include "Core/Log.h"

#include <stb_image.h>

#include <cstring>

namespace Nightbird::Editor
{
	bool StbTextureImporter::SupportsExtension(const std::string& extension) const
	{
		return	extension == ".jpg" || extension == ".jpeg"	|| extension == ".png"	||
				extension == ".tga" || extension == ".bmp"	|| extension == ".psd"	||
				extension == ".gif" || extension == ".hdr"	|| extension == ".pic";
	}

	std::shared_ptr<Core::Texture> StbTextureImporter::Load(const AssetInfo& assetInfo)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(false);

		unsigned char* pixels = stbi_load(assetInfo.path.string().c_str(), &width, &height, &channels, 4);

		if (!pixels)
		{
			Core::Log::Error("StbTextureImporter: Failed to load: " + assetInfo.path.string() + ", " + stbi_failure_reason());
			return nullptr;
		}

		std::vector<uint8_t> data(pixels, pixels + (width * height * 4));
		stbi_image_free(pixels);
		
		return std::make_shared<Core::Texture>(static_cast<uint32_t>(width), static_cast<uint32_t>(height), Core::TextureFormat::RGBA8, std::move(data));
	}
}
