#pragma once

#include "Core/SpatialObject.h"
#include "Core/AssetRef.h"
#include "Core/Mesh.h"

namespace Nightbird::Core
{
	class MeshInstance : public SpatialObject
	{
	public:
		NB_TYPE()

		using SpatialObject::SpatialObject;
		
		void ResolveAssets(AssetManager& assetManager) override;
		void EnterScene() override;
		
		AssetRef<Mesh> m_Mesh;
	};
}
