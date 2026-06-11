#include "Core/AudioLoader.h"

#include "Core/BinaryReader.h"

#include "Core/AudioAsset.h"
#include "Core/Log.h"

namespace Nightbird::Core
{
	std::shared_ptr<AudioAsset> AudioLoader::Load(const std::string& cookedDir, const uuids::uuid& uuid)
	{
		std::string path = cookedDir + "/" + uuids::to_string(uuid) + ".nbaudio";
		BinaryReader reader(path);
		if (!reader.IsValid())
		{
			Log::Error("AudioLoader: Failed to open: " + path);
			return nullptr;
		}

		// Validate type
		uint8_t type[4] = {};
		reader.ReadRawBytes(type, 4);
		if (type[0] != 'A' || type[1] != 'D' || type[2] != 'I' || type[3] != 'O')
		{
			Log::Error("AudioLoader: Invalid type signature in: " + path);
			return nullptr;
		}

		// Version
		uint32_t version = reader.ReadUInt32();
		if (version != 1)
		{
			Log::Error("AudioLoader: Unsupported version: " + std::to_string(version));
			return nullptr;
		}

		// Encoding, channels, padding
		AudioEncoding encoding = static_cast<AudioEncoding>(reader.ReadUInt8());
		uint8_t channels = reader.ReadUInt8();
		bool planar = reader.ReadUInt8() != 0;
		reader.ReadUInt8(); // Padding

		// Sample rate, frame count
		uint32_t sampleRate = reader.ReadUInt32();
		uint32_t frameCount = reader.ReadUInt32();

		switch (encoding)
		{
		case AudioEncoding::PCM16:
		{
			size_t totalSamples = static_cast<size_t>(channels) * frameCount;
			std::vector<uint16_t> rawData(totalSamples);
			reader.ReadRawBytes(reinterpret_cast<uint8_t*>(rawData.data()), totalSamples * sizeof(int16_t));

			std::vector<std::vector<uint8_t>> channelData(channels);

			if (planar)
			{
				for (uint8_t channel = 0; channel < channels; ++channel)
				{
					channelData[channel].resize(frameCount * sizeof(int16_t));
					std::memcpy(channelData[channel].data(), rawData.data() + channel * frameCount, frameCount * sizeof(int16_t));
				}
			}
			else
			{
				channelData[0].resize(totalSamples * sizeof(int16_t));
				std::memcpy(channelData[0].data(), rawData.data(), totalSamples * sizeof(int16_t));
			}

			return std::make_shared<AudioAsset>(sampleRate, frameCount, channels, encoding, std::move(channelData));
		}
		case AudioEncoding::DSP_ADPCM:
		{
			std::vector<uint32_t> channelSizes(channels);
			for (uint8_t channel = 0; channel < channels; ++channel)
				channelSizes[channel] = reader.ReadUInt32();

			std::vector<std::vector<uint8_t>> channelData(channels);
			for (uint8_t channel = 0; channel < channels; ++channel)
			{
				channelData[channel].resize(channelSizes[channel]);
				reader.ReadRawBytes(channelData[channel].data(), channelSizes[channel]);
			}

			return std::make_shared<AudioAsset>(sampleRate, frameCount, channels, encoding, std::move(channelData));
		}
		default:
			Log::Error("AudioLoader: Unknown encoding: " + std::to_string(static_cast<uint8_t>(encoding)));
			return nullptr;
		}
	}
}
