#pragma once

#include "Core/SceneObject.h"
#include "Core/AssetRef.h"
#include "Core/Cubemap.h"

namespace Nightbird::Core
{
	class NB_API Skybox : public SceneObject
	{
		NB_TYPE()
		using SceneObject::SceneObject;

		void ResolveAssets(AssetManager& assetManager) override;

		AssetRef<Cubemap> m_Cubemap;
	};
}