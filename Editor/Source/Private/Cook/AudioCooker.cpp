#include "Cook/AudioCooker.h"

#include "Cook/BinaryWriter.h"

#include "Core/AudioAsset.h"
#include "Core/Log.h"

#include <dr_wav.h>
#include <dr_flac.h>

#include <fstream>
#include <cstdint>

namespace Nightbird::Editor
{
	static std::filesystem::path GetVGAudioCliPath()
	{
		const char* envPath = std::getenv("NIGHTBIRD_PATH");
		if (envPath)
		{
			return std::filesystem::path(envPath) / "Tools" / "VGAudioCli" / "VGAudioCli.dll";
		}

		return std::filesystem::path("Tools") / "VGAudioCli" / "VGAudioCli.dll";
	}

	static std::vector<uint8_t> ReadFile(const std::filesystem::path& path)
	{
		std::ifstream file(path, std::ios::binary);
		if (!file)
			return {};
		return std::vector<uint8_t>(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
	}

	static void ByteSwapPCM16(int16_t* data, uint64_t sampleCount)
	{
		for (uint64_t i = 0; i < sampleCount; i++)
		{
			uint8_t* bytes = reinterpret_cast<uint8_t*>(&data[i]);
			std::swap(bytes[0], bytes[1]);
		}
	}

	void AudioCooker::Cook(const std::filesystem::path& assetPath, const uuids::uuid& uuid, const std::filesystem::path& outputDir, CookTarget target, Endianness endianness)
	{
		std::filesystem::create_directories(outputDir);
		std::filesystem::path outputPath = outputDir / (uuids::to_string(uuid) + ".nbaudio");

		uint32_t sampleRate = 0;
		uint32_t frameCount = 0;
		uint8_t channels = 0;
		Core::AudioEncoding encoding;
		bool planar = false;
		std::vector<uint8_t> data;

		switch (target)
		{
			case CookTarget::Desktop:
				encoding = Core::AudioEncoding::PCM16;
				planar = false;
				data = CookPCM16(assetPath, sampleRate, frameCount, channels, endianness, planar);
				break;
			case CookTarget::WiiU:
				encoding = Core::AudioEncoding::PCM16;
				planar = true;
				data = CookPCM16(assetPath, sampleRate, frameCount, channels, endianness, planar);
				break;
			case CookTarget::N3DS:
				encoding = Core::AudioEncoding::DSP_ADPCM;
				data = CookDSPADPCM(assetPath, uuid, sampleRate, frameCount, channels);
				break;
			default:
				Core::Log::Error("AudioCooker: Unknown target");
				return;
		}

		if (data.empty())
		{
			Core::Log::Error("AudioCooker: Failed to cook: " + assetPath.string());
			return;
		}

		BinaryWriter writer(outputPath, endianness);

		// Type
		writer.WriteUInt8('A');
		writer.WriteUInt8('D');
		writer.WriteUInt8('I');
		writer.WriteUInt8('O');

		// Version
		writer.WriteUInt32(1);

		// Encoding
		writer.WriteUInt8(static_cast<uint8_t>(encoding));

		// Channels
		writer.WriteUInt8(channels);

		// Planar
		writer.WriteUInt8(planar ? 1 : 0);

		// Padding
		writer.WriteUInt8(0);

		// Sample rate
		writer.WriteUInt32(sampleRate);

		// Frame count
		writer.WriteUInt32(frameCount);

		// Data
		writer.WriteRawBytes(data.data(), data.size());

		Core::Log::Info("Cooked audio: " + outputPath.string());
	}

	std::vector<uint8_t> AudioCooker::CookPCM16(const std::filesystem::path& assetPath, uint32_t& outSampleRate, uint32_t& outFrameCount, uint8_t& outChannels, Endianness endianness, bool planar)
	{
		std::string extension = assetPath.extension().string();
		for (auto& c : extension)
			c = tolower(c);

		unsigned int channels, sampleRate;

		uint64_t frameCount = 0;
		int16_t* decoded = nullptr;

		// Collect metadata
		if (extension == ".wav")
		{
			decoded = drwav_open_file_and_read_pcm_frames_s16(assetPath.string().c_str(), &channels, &sampleRate, reinterpret_cast<drwav_uint64*>(&frameCount), nullptr);
		}
		else if (extension == ".flac")
		{
			decoded = drflac_open_file_and_read_pcm_frames_s16(assetPath.string().c_str(), &channels, &sampleRate, reinterpret_cast<drflac_uint64*>(&frameCount), nullptr);
		}
		else
		{
			Core::Log::Error("AudioCooker: Unsupported format for PCM16 cooking: " + extension);
			return {};
		}

		if (!decoded)
		{
			Core::Log::Error("AudioCooker: Failed to decode audio: " + assetPath.string());
			return {};
		}

		outSampleRate = sampleRate;
		outFrameCount = static_cast<uint32_t>(frameCount);
		outChannels = static_cast<uint8_t>(channels);

		uint64_t totalSamples = frameCount * channels;

		if (endianness == Endianness::Big)
			ByteSwapPCM16(decoded, totalSamples);

		std::vector<uint8_t> result;
		result.resize(totalSamples * sizeof(int16_t));

		if (planar && channels > 1)
		{
			int16_t* planarPtr = reinterpret_cast<int16_t*>(result.data());
			for (uint32_t frame = 0; frame < frameCount; ++frame)
			{
				for (uint8_t channel = 0; channel < channels; ++channel)
				{
					planarPtr[channel * frameCount + frame] = decoded[frame * channels + channel];
				}
			}
		}
		else
		{
			std::memcpy(result.data(), decoded, totalSamples * sizeof(int16_t));
		}

		if (extension == ".wav")
			drwav_free(decoded, nullptr);
		else if (extension == ".flac")
			drflac_free(decoded, nullptr);
		else
			free(decoded);

		return result;
	}

	std::vector<uint8_t> AudioCooker::CookDSPADPCM(const std::filesystem::path& assetPath, const uuids::uuid& uuid, uint32_t& outSampleRate, uint32_t& outFrameCount, uint8_t& outChannels)
	{
		std::string extension = assetPath.extension().string();
		for (auto& c : extension)
			c = tolower(c);

		std::filesystem::path tempDir = std::filesystem::temp_directory_path();
		std::string uuidString = uuids::to_string(uuid);

		std::filesystem::path wavPath;
		bool tempWav = false;

		if (extension == ".wav")
		{
			drwav wav;
			if (!drwav_init_file(&wav, assetPath.string().c_str(), nullptr))
			{
				Core::Log::Error("AudioCooker: Failed to open WAV for metadata: " + assetPath.string());
				return {};
			}

			outSampleRate = wav.sampleRate;
			outFrameCount = static_cast<uint32_t>(wav.totalPCMFrameCount);
			outChannels = static_cast<uint8_t>(wav.channels);

			drwav_uninit(&wav);

			wavPath = assetPath;
		}
		else if (extension == ".flac")
		{
			unsigned int channels, sampleRate;

			drflac_uint64 frameCount;

			drflac_int16* decoded = drflac_open_file_and_read_pcm_frames_s16(assetPath.string().c_str(), &channels, &sampleRate, &frameCount, nullptr);

			if (!decoded)
			{
				Core::Log::Error("AudioCooker: Failed to decode FLAC: " + assetPath.string());
				return {};
			}

			outSampleRate = sampleRate;
			outFrameCount = static_cast<uint32_t>(frameCount);
			outChannels = static_cast<uint8_t>(channels);

			wavPath = tempDir / (uuidString + "_temp.wav");
			tempWav = true;

			drwav_data_format format{};
			format.container = drwav_container_riff;
			format.format = DR_WAVE_FORMAT_PCM;
			format.channels = channels;
			format.sampleRate = sampleRate;
			format.bitsPerSample = 16;

			drwav wav;
			drwav_init_file_write(&wav, wavPath.string().c_str(), &format, nullptr);
			drwav_write_pcm_frames(&wav, frameCount, decoded);
			drwav_uninit(&wav);

			drflac_free(decoded, nullptr);

			Core::Log::Info("AudioCooker: Converted FLAC to temp WAV: " + wavPath.string());
		}
		else
		{
			Core::Log::Error("AudioCooker: Unsupported format for DSP-ADPCM cooking: " + extension);
			return {};
		}

		std::filesystem::path vgaudio = GetVGAudioCliPath();
		std::vector<std::vector<uint8_t>> channelBlobs;

		for (uint8_t channel = 0; channel < outChannels; channel++)
		{
			std::filesystem::path dspPath = tempDir / (uuidString + "_channel" + std::to_string(channel) + ".dsp");

			std::string command = "dotnet " + vgaudio.string() +
				" -i:" + std::to_string(channel) +
				" " + wavPath.string() +
				" -o " + dspPath.string();

			Core::Log::Info("AudioCooker: " + command);
			int result_code = std::system(command.c_str());
			if (result_code != 0)
			{
				Core::Log::Error("AudioCooker: VGAudioCli failed for channel " + std::to_string(channel));
				if (tempWav)
					std::filesystem::remove(wavPath);
				return {};
			}

			std::vector<uint8_t> dspData = ReadFile(dspPath);
			// DSP data smaller than header
			if (dspData.size() < 96)
			{
				Core::Log::Error("AudioCooker: DSP file too small for channel " + std::to_string(channel));
				std::filesystem::remove(dspPath);
				if (tempWav)
					std::filesystem::remove(wavPath);
				return {};
			}

			channelBlobs.push_back(std::move(dspData));
			std::filesystem::remove(dspPath);
		}

		if (tempWav)
			std::filesystem::remove(wavPath);

		std::vector<uint8_t> result;

		// Write blob sizes
		for (const auto& blob : channelBlobs)
		{
			uint32_t size = static_cast<uint32_t>(blob.size());
			result.insert(result.end(), reinterpret_cast<uint8_t*>(&size), reinterpret_cast<uint8_t*>(&size) + 4);
		}

		// Write blobs
		for (const auto& blob : channelBlobs)
			result.insert(result.end(), blob.begin(), blob.end());

		return result;
	}
}
