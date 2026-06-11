#pragma once

#include "Import/AudioImporter.h"

namespace Nightbird::Editor
{
	class DrLibsAudioImporter : public AudioImporter
	{
	public:
		std::string GetName() const override { return "dr_libs_audio"; }
		bool SupportsExtension(const std::string& extension) const override;
		std::shared_ptr<Core::AudioAsset> Load(const AssetInfo& assetInfo) override;
	};
}
