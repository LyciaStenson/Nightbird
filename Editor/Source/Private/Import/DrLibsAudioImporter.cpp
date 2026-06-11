#include "Import/DrLibsAudioImporter.h"

#include "Core/AudioAsset.h"
#include "Core/Log.h"

#define DR_WAV_IMPLEMENTATION
#include <dr_wav.h>
#define DR_FLAC_IMPLEMENTATION
#include <dr_flac.h>

#include <cstring>

namespace Nightbird::Editor
{
	bool DrLibsAudioImporter::SupportsExtension(const std::string& extension) const
	{
		return extension == ".wav" || extension == ".flac";
	}

	std::shared_ptr<Core::AudioAsset> DrLibsAudioImporter::Load(const AssetInfo& assetInfo)
	{
		std::string extension = assetInfo.path.extension().string();
		for (auto& c : extension)
			c = tolower(c);

		unsigned int channels, sampleRate;

		if (extension == ".wav")
		{
			drwav_uint64 frameCount;
			drwav_int16* decoded = drwav_open_file_and_read_pcm_frames_s16(assetInfo.path.string().c_str(), &channels, &sampleRate, &frameCount, nullptr);
			if (!decoded)
			{
				Core::Log::Error("DrLibsAudioImporter: Failed to decode WAV: " + assetInfo.path.string());
				return nullptr;
			}
			uint64_t sampleCount = frameCount * channels;
			std::vector<uint8_t> pcmData(sampleCount * sizeof(drwav_int16));
			std::memcpy(pcmData.data(), decoded, pcmData.size());
			drwav_free(decoded, nullptr);
			std::vector<std::vector<uint8_t>> channelData;
			channelData.push_back(std::move(pcmData));
			return std::make_shared<Core::AudioAsset>(sampleRate, static_cast<uint32_t>(frameCount), static_cast<uint8_t>(channels), Core::AudioEncoding::PCM16, std::move(channelData));
		}
		else if (extension == ".flac")
		{
			drflac_uint64 frameCount;
			drflac_int16* decoded = drflac_open_file_and_read_pcm_frames_s16(assetInfo.path.string().c_str(), &channels, &sampleRate, &frameCount, nullptr);
			if (!decoded)
			{
				Core::Log::Error("DrLibsAudioImporter: Failed to decode FLAC: " + assetInfo.path.string());
				return nullptr;
			}
			uint64_t sampleCount = frameCount * channels;
			std::vector<uint8_t> pcmData(sampleCount * sizeof(drflac_int16));
			std::memcpy(pcmData.data(), decoded, pcmData.size());
			drflac_free(decoded, nullptr);
			std::vector<std::vector<uint8_t>> channelData;
			channelData.push_back(std::move(pcmData));
			return std::make_shared<Core::AudioAsset>(sampleRate, static_cast<uint32_t>(frameCount), static_cast<uint8_t>(channels), Core::AudioEncoding::PCM16, std::move(channelData));
		}

		Core::Log::Error("DrLibsAudioImporter: Unsupported format: " + extension);
		return nullptr;
	}
}
