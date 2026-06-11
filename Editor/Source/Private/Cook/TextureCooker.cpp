#include "Cook/TextureCooker.h"

#include "Cook/BinaryWriter.h"

#include "Core/Texture.h"
#include "Core/Log.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize2.h>

namespace Nightbird::Editor
{
	static bool WritePNG(const std::filesystem::path& path, uint32_t width, uint32_t height, const std::vector<uint8_t>& pixels)
	{
		int result = stbi_write_png(path.string().c_str(), width, height, 4, pixels.data(), width * 4);
		return result != 0;
	}

#ifdef _WIN32
	static std::string ToMSys2Path(const std::string& path)
	{
		if (path.empty())
			return "";

		char driveLetter = path[0];
		if (driveLetter < 'A' || driveLetter > 'Z')
			return path;

		std::string msysPath = path.substr(2);
		std::replace(msysPath.begin(), msysPath.end(), '\\', '/');
		msysPath = "/" + std::string(1, std::tolower(driveLetter)) + msysPath;
		return msysPath;
	}
#endif

	static uint32_t NextPow2(uint32_t v)
	{
		v--;			// E.g. 1024 -> 1023
		v |= v >> 1;	// Smear highest set bit rightward, 1 position
		v |= v >> 2;	// Smear 2 more
		v |= v >> 4;	// Smear 4 more
		v |= v >> 8;	// Smear 8 more
		v |= v >> 16;	// Smear 16 more so all bits below highest are 1
		return ++v;		// Add 1 to get the next power of two: e.g. 1024
	}

	void TextureCooker::Cook(const Core::Texture& texture, const uuids::uuid& uuid, const std::filesystem::path& outputDir, CookTarget target, Endianness endianness)
	{
		std::filesystem::create_directories(outputDir);

		std::filesystem::path outputPath = outputDir / (uuids::to_string(uuid) + ".nbtexture");

		Core::TextureFormat format;
		std::vector<uint8_t> data;

		switch (target)
		{
		case CookTarget::Desktop:
			// Fall through
		case CookTarget::WiiU:
			format = Core::TextureFormat::RGBA8;
			data = CookRGBA(texture);
			break;
		case CookTarget::N3DS:
			format = Core::TextureFormat::T3X;
			data = CookT3X(texture, uuid);
			break;
		default:
			format = Core::TextureFormat::RGBA8;
			data = CookRGBA(texture);
			break;
		}

		if (data.empty())
		{
			Core::Log::Error("TextureCooker: Failed to cook: " + outputPath.string());
			return;
		}

		BinaryWriter writer(outputPath, endianness);

		// Type signature
		writer.WriteUInt8('T');
		writer.WriteUInt8('E');
		writer.WriteUInt8('X');
		writer.WriteUInt8('T');

		// Verison
		writer.WriteUInt32(1);

		// Dimensions
		writer.WriteUInt32(texture.GetWidth());
		writer.WriteUInt32(texture.GetHeight());

		// Format
		writer.WriteUInt32(static_cast<uint32_t>(format));

		// Data size
		writer.WriteUInt32(static_cast<uint32_t>(data.size()));

		// Pixels
		writer.WriteRawBytes(data.data(), data.size());

		Core::Log::Info("TextureCooker: Cooked texture: " + outputPath.string());
	}

	std::vector<uint8_t> TextureCooker::CookRGBA(const Core::Texture& texture)
	{
		const auto& data = texture.GetData();
		return std::vector<uint8_t>(data.begin(), data.end());
	}

	std::vector<uint8_t> TextureCooker::CookT3X(const Core::Texture& texture, const uuids::uuid& uuid)
	{
		std::filesystem::path tempPath = std::filesystem::temp_directory_path();

		std::filesystem::path inputPath = tempPath / (uuids::to_string(uuid) + ".png");
		std::filesystem::path outputPath = tempPath / (uuids::to_string(uuid) + ".t3x");

		const auto& srcPixels = texture.GetData();
		std::vector<uint8_t> pixels = srcPixels;

		for (size_t i = 0; i < pixels.size(); i += 4)
		{
			// Force opaque
			pixels[i + 3] = 255;
		}

		uint32_t srcWidth = texture.GetWidth();
		uint32_t srcHeight = texture.GetHeight();
		uint32_t dstWidth = std::min(NextPow2(srcWidth), 1024u);
		uint32_t dstHeight = std::min(NextPow2(srcHeight), 1024u);

		if (dstWidth != srcWidth || dstHeight != srcHeight)
		{
			std::vector<uint8_t> resized(dstWidth * dstHeight * 4);
			stbir_resize_uint8_linear(pixels.data(), srcWidth, srcHeight, 0, resized.data(), dstWidth, dstHeight, 0, STBIR_RGBA);
			pixels = std::move(resized);
		}

		if (!WritePNG(inputPath, dstWidth, dstHeight, pixels))
		{
			Core::Log::Error("TextureCooker: Failed to write temporator PNG for tex3ds.");
			return {};
		}

#ifdef _WIN32
		std::string msys2InputPath = ToMSys2Path(inputPath.string());
		std::string msys2OutputPath = ToMSys2Path(outputPath.string());
		std::string command = "cmd /c \"\"C:\\devkitPro\\msys2\\usr\\bin\\bash.exe\" -l -c \"tex3ds '" + msys2InputPath + "' -f rgba5551 -z auto -o '" + msys2OutputPath + "'\"\"";
#else
		std::string command = "tex3ds \"" + inputPath.string() + "\" -f rgba5551 -z auto -o \"" + outputPath.string() + "\"";
#endif

		int result = std::system(command.c_str());
		if (result != 0)
		{
			Core::Log::Error("TextureCooker: tex3ds failed.");
			return {};
		}

		std::ifstream file(outputPath, std::ios::binary);
		if (!file)
		{
			Core::Log::Error("TextureCooker: Failed to open tex3ds output.");
			return {};
		}

		std::vector<uint8_t> data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

		file.close();

		std::filesystem::remove(inputPath);
		std::filesystem::remove(outputPath);
		return data;
	}
}
