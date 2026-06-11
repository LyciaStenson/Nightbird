#include "Core/Skybox.h"

#include "Core/AssetManager.h"

NB_REFLECT(Nightbird::Core::Skybox, NB_PARENT(Nightbird::Core::SceneObject), NB_FACTORY(Nightbird::Core::Skybox),
	NB_FIELD(m_Cubemap)
)

volatile int nb_link_Skybox = 0;

namespace Nightbird::Core
{
	void Skybox::ResolveAssets(AssetManager& assetManager)
	{
		if (!m_Cubemap.GetUUID().is_nil())
			m_Cubemap.Resolve(assetManager.Load<Cubemap>(m_Cubemap.GetUUID()));
	}
}