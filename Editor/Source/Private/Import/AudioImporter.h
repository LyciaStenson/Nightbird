#pragma once

#include "Import/Importer.h"

#include <memory>

namespace Nightbird::Core
{
	class AudioAsset;
}

namespace Nightbird::Editor
{
	struct AssetInfo;

	class AudioImporter : public Importer
	{
	public:
		AudioImporter* AsAudioImporter() override { return this; }

		virtual std::shared_ptr<Core::AudioAsset> Load(const AssetInfo& assetInfo) = 0;
	};
}
