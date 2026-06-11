#include "Core/MeshInstance.h"

NB_REFLECT(Nightbird::Core::MeshInstance, NB_PARENT(Nightbird::Core::SpatialObject), NB_FACTORY(Nightbird::Core::MeshInstance),
	NB_FIELD(m_Mesh)
)

namespace Nightbird::Core
{
	void MeshInstance::ResolveAssets(AssetManager& assetManager)
	{
		m_Mesh.Resolve(assetManager.Load<Mesh>(m_Mesh.GetUUID()));
	}

	void MeshInstance::EnterScene()
	{

	}
}
